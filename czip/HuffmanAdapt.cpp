/*
* Descent 3 
* Copyright (C) 2024 Parallax Software
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.

--- HISTORICAL COMMENTS FOLLOW ---

 * $Logfile: /DescentIII/Main/czip/HuffmanAdapt.cpp $
 * $Revision: 2 $
 * $Date: 8/27/98 3:26p $
 * $Author: Jeff $
 *
 * Huffman-Adaptive compression functions
 *
 * $Log: /DescentIII/Main/czip/HuffmanAdapt.cpp $
 *
 * 2     8/27/98 3:26p Jeff
 * intial creation
 *
 * 1     8/27/98 3:26p Jeff
 *
 * $NoKeywords: $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "CZip.h"

#define END_OF_STREAM 256
#define ESCAPE 257
#define ROOT_NODE 0
#define MAX_WEIGHT 0x8000

// void calculate_rows(tHATree *tree,int node,int level);
// int calculate_columns(tHATree *tree,int node,int starting_guess);
// int find_minimum_column(tHATree *tree,int node,int max_row);
// void rescale_columns(int factor);

/////////////////////////////////////////////
int CZip::ha_CompressFile(tVirtualFile *input, BITFILE *output) {
  int original_pos = VFtell(output->file);
  int c;
  ha_InitializeTree(&Tree);
  while ((c = VFgetc(input)) != EOF) {
    ha_EncodeSymbol(&Tree, c, output);
    ha_UpdateModel(&Tree, c);
  }
  ha_EncodeSymbol(&Tree, END_OF_STREAM, output);
  FlushOutputBitFile(output);
  int size = VFtell(output->file) - original_pos;
  return size;
}

void CZip::ha_PrepareCompress(void) {
  ok_to_raw_write = true;
  ha_InitializeTree(&Tree);
}

void CZip::ha_WriteRawByte(ubyte data, BITFILE *output) {
  if (!ok_to_raw_write)
    return;
  ha_EncodeSymbol(&Tree, data, output);
  ha_UpdateModel(&Tree, data);
}

void CZip::ha_CloseRawCompress(BITFILE *output) {
  ha_EncodeSymbol(&Tree, END_OF_STREAM, output);
  FlushOutputBitFile(output);
  ok_to_raw_write = false;
}

void CZip::ha_ExpandFile(BITFILE *input, tVirtualFile *output) {
  int c;
  ha_InitializeTree(&Tree);
  while ((c = ha_DecodeSymbol(&Tree, input)) != END_OF_STREAM) {
    if (VFputc(c, output) == EOF) {
      // fatal error
    }
    ha_UpdateModel(&Tree, c);
  }
}

void CZip::ha_PrepareDecompress(void) {
  ok_to_raw_read = true;
  ha_InitializeTree(&Tree);
}

bool CZip::ha_ReadRawByte(ubyte *data, BITFILE *input) {
  if (!ok_to_raw_read)
    return false;

  int c;
  c = ha_DecodeSymbol(&Tree, input);

  if (c == END_OF_STREAM) {
    ok_to_raw_read = false;
    return false;
  }
  ha_UpdateModel(&Tree, c);
  *data = c;
  return true;
}

void CZip::ha_CloseRawDecompress(void) { ok_to_raw_read = false; }

void CZip::ha_InitializeTree(tHATree *tree) {
  int i;

  tree->nodes[ROOT_NODE].child = ROOT_NODE + 1;
  tree->nodes[ROOT_NODE].child_is_leaf = false;
  tree->nodes[ROOT_NODE].weight = 2;
  tree->nodes[ROOT_NODE].parent = -1;

  tree->nodes[ROOT_NODE + 1].child = END_OF_STREAM;
  tree->nodes[ROOT_NODE + 1].child_is_leaf = true;
  tree->nodes[ROOT_NODE + 1].weight = 1;
  tree->nodes[ROOT_NODE + 1].parent = ROOT_NODE;
  tree->leaf[END_OF_STREAM] = ROOT_NODE + 1;

  tree->nodes[ROOT_NODE + 2].child = ESCAPE;
  tree->nodes[ROOT_NODE + 2].child_is_leaf = true;
  tree->nodes[ROOT_NODE + 2].weight = 1;
  tree->nodes[ROOT_NODE + 2].parent = ROOT_NODE;
  tree->leaf[ESCAPE] = ROOT_NODE + 2;
  tree->next_free_node = ROOT_NODE + 3;
  for (i = 0; i < END_OF_STREAM; i++)
    tree->leaf[i] = -1;
}

void CZip::ha_EncodeSymbol(tHATree *tree, uint32_t c, BITFILE *output) {
  uint32_t code;
  uint32_t current_bit;
  int code_size;
  int current_node;

  code = 0;
  current_bit = 1;
  code_size = 0;
  current_node = tree->leaf[c];
  if (current_node == -1)
    current_node = tree->leaf[ESCAPE];
  while (current_node != ROOT_NODE) {
    if ((current_node & 1) == 0)
      code |= current_bit;
    current_bit <<= 1;
    code_size++;
    current_node = tree->nodes[current_node].parent;
  }
  OutputBits(output, code, code_size);
  if (tree->leaf[c] == -1) {
    OutputBits(output, (uint32_t)c, 8);
    ha_add_new_node(tree, c);
  }
}

int CZip::ha_DecodeSymbol(tHATree *tree, BITFILE *input) {
  int current_node;
  int c;

  current_node = ROOT_NODE;
  while (!tree->nodes[current_node].child_is_leaf) {
    current_node = tree->nodes[current_node].child;
    current_node += InputBit(input);
  }
  c = tree->nodes[current_node].child;
  if (c == ESCAPE) {
    c = (int)InputBits(input, 8);
    ha_add_new_node(tree, c);
  }
  return c;
}

void CZip::ha_UpdateModel(tHATree *tree, int c) {
  int current_node;
  int new_node;

  if (tree->nodes[ROOT_NODE].weight == MAX_WEIGHT)
    ha_RebuildTree(tree);
  current_node = tree->leaf[c];
  while (current_node != -1) {
    tree->nodes[current_node].weight++;
    for (new_node = current_node; new_node > ROOT_NODE; new_node--) {
      if (tree->nodes[new_node - 1].weight >= tree->nodes[current_node].weight)
        break;
    }
    if (current_node != new_node) {
      ha_swap_nodes(tree, current_node, new_node);
      current_node = new_node;
    }
    current_node = tree->nodes[current_node].parent;
  }
}

void CZip::ha_RebuildTree(tHATree *tree) {
  int i, j, k;
  uint32_t weight;
  j = tree->next_free_node - 1;
  for (i = j; i >= ROOT_NODE; i--) {
    if (tree->nodes[i].child_is_leaf) {
      tree->nodes[j] = tree->nodes[i];
      tree->nodes[j].weight = (tree->nodes[j].weight + 1) / 2;
      j--;
    }
  }

  for (i = tree->next_free_node - 2; j >= ROOT_NODE; i -= 2, j--) {
    k = i + 1;
    tree->nodes[j].weight = tree->nodes[i].weight + tree->nodes[k].weight;
    weight = tree->nodes[j].weight;
    tree->nodes[j].child_is_leaf = false;
    for (k = j + 1; weight < tree->nodes[k].weight; k++)
      ;
    k--;
    memmove(&tree->nodes[j], &tree->nodes[j + 1], (k - j) * sizeof(tHANode));
    tree->nodes[k].weight = weight;
    tree->nodes[k].child = i;
    tree->nodes[k].child_is_leaf = false;
  }

  for (i = tree->next_free_node - 1; i >= ROOT_NODE; i--) {
    if (tree->nodes[i].child_is_leaf) {
      k = tree->nodes[i].child;
      tree->leaf[k] = i;
    } else {
      k = tree->nodes[i].child;
      tree->nodes[k].parent = tree->nodes[k + 1].parent = i;
    }
  }
}

void CZip::ha_swap_nodes(tHATree *tree, int i, int j) {
  tHANode temp;

  if (tree->nodes[i].child_is_leaf)
    tree->leaf[tree->nodes[i].child] = j;
  else {
    tree->nodes[tree->nodes[i].child].parent = j;
    tree->nodes[tree->nodes[i].child + 1].parent = j;
  }

  if (tree->nodes[j].child_is_leaf)
    tree->leaf[tree->nodes[j].child] = i;
  else {
    tree->nodes[tree->nodes[j].child].parent = i;
    tree->nodes[tree->nodes[j].child + 1].parent = i;
  }

  temp = tree->nodes[i];
  tree->nodes[i] = tree->nodes[j];
  tree->nodes[i].parent = temp.parent;
  temp.parent = tree->nodes[j].parent;
  tree->nodes[j] = temp;
}

void CZip::ha_add_new_node(tHATree *tree, int c) {
  int lightest_node, new_node, zero_weight_node;

  lightest_node = tree->next_free_node - 1;
  new_node = tree->next_free_node;
  zero_weight_node = tree->next_free_node + 1;
  tree->next_free_node += 2;

  tree->nodes[new_node] = tree->nodes[lightest_node];
  tree->nodes[new_node].parent = lightest_node;
  tree->leaf[tree->nodes[new_node].child] = new_node;

  tree->nodes[lightest_node].child = new_node;
  tree->nodes[lightest_node].child_is_leaf = false;

  tree->nodes[zero_weight_node].child = c;
  tree->nodes[zero_weight_node].child_is_leaf = true;
  tree->nodes[zero_weight_node].weight = 0;
  tree->nodes[zero_weight_node].parent = lightest_node;
  tree->leaf[c] = zero_weight_node;
}

// void calculate_rows(tHATree *tree,int node,int level);
// int calculate_columns(tHATree *tree,int node,int starting_guess);
// int find_minimum_column(tHATree *tree,int node,int max_row);
// void rescale_columns(int factor);
