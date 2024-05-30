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

 * $Logfile: /DescentIII/main/debuggraph.cpp $
 * $Revision: 4 $
 * $Date: 9/23/99 12:02p $
 * $Author: Jeff $
 *
 * Debug graph visual log
 *
 * $Log: /DescentIII/main/debuggraph.cpp $
 *
 * 4     9/23/99 12:02p Jeff
 * include stdlib for atexit
 *
 * 3     5/10/99 10:21p Ardussi
 * changes to compile on Mac
 *
 * 2     4/04/99 8:14p Jeff
 * added debug graph stuff
 *
 * $NoKeywords: $
 */

#include <stdlib.h>
#include "debuggraph.h"
#include "grdefs.h"
#include "mono.h"
#include <memory.h>
#include "pserror.h"
#include "bitmap.h"
#include "renderer.h"
#include "pstypes.h"
#include "stringtable.h"
#include "newui.h"
#include "newui_core.h"
#include "mem.h"
#include <string.h>
#include "game.h"

#define DATA_TYPE_INT 0
#define DATA_TYPE_FLOAT 1
#define NUM_FRAMES_TO_DISPLAY 128
#define MAX_GRAPH_NODES 16
#define MAX_NAME 38

int graph_bmp = -1;
int graph_mask = 0;
int graph_num_nodes = 0;

struct tGraphColor {
  const char *color_name;
  ddgr_color color;
};

tGraphColor GraphColors[MAX_GRAPH_NODES] = {
    {"Green", GR_RGB(40, 255, 40)},     {"Red", GR_RGB(255, 40, 40)},     {"Blue", GR_RGB(40, 40, 255)},
    {"Yellow", GR_RGB(255, 255, 40)},   {"Purple", GR_RGB(255, 40, 255)}, {"White", GR_RGB(255, 255, 255)},
    {"Lt.Grey", GR_RGB(180, 180, 180)}, {"Dk.Grey", GR_RGB(80, 80, 80)},  {"Orange", GR_RGB(255, 140, 5)},
    {"Pink", GR_RGB(250, 147, 147)},    {"Brown", GR_RGB(119, 68, 2)},    {"Dk.Blue", GR_RGB(3, 20, 124)},
    {"Dk.Red", GR_RGB(100, 2, 2)},      {"Dk.Green", GR_RGB(2, 100, 9)},  {"Turquoise", GR_RGB(14, 229, 202)},
    {"Dk.Purple", GR_RGB(100, 4, 104)}};

struct tAddDebugGraph {
  uint8_t data_input;
  int flags;

  union {
    int i_min_value;
    float f_min_value;
  };
  union {
    int i_max_value;
    float f_max_value;
  };

};

class tGraphNode {
public:
  tGraphNode() {}
  ~tGraphNode() {}

  ddgr_color color;

  void Initialize(tAddDebugGraph *data);
  void Update(int value);
  void Update(float value);
  void Render(void);

  char name[MAX_NAME];
  tAddDebugGraph init_data;

private:
  int cur_write_pos;
  int first_pos;
  int num_items_stored;

  union {
    int i_data[NUM_FRAMES_TO_DISPLAY];
    float f_data[NUM_FRAMES_TO_DISPLAY];
  };
};

struct tDebugGraphNode {
  tGraphNode *node;
  tDebugGraphNode *next;
};

tDebugGraphNode *DebugGraphNode_root = NULL;

tGraphNode *DebugGraph_AddNode(void) {
  if (graph_num_nodes == MAX_GRAPH_NODES)
    return NULL;

  tDebugGraphNode *curr = DebugGraphNode_root;

  if (!curr) {
    curr = DebugGraphNode_root = (tDebugGraphNode *)mem_malloc(sizeof(tDebugGraphNode));
  } else {
    while (curr->next) {
      curr = curr->next;
    }
    curr->next = (tDebugGraphNode *)mem_malloc(sizeof(tDebugGraphNode));
    curr = curr->next;
  }

  if (!curr) {
    Error("Out of Memory");
  }

  graph_num_nodes++;
  curr->next = NULL;
  curr->node = new tGraphNode;

  if (!curr->node) {
    Error("Out of Memory");
  }

  return curr->node;
}

tGraphNode *DebugGraph_FindNode(int index) {
  if (index >= graph_num_nodes)
    return NULL;
  tDebugGraphNode *curr = DebugGraphNode_root;
  int ci = 0;
  while (curr) {
    if (ci == index) {
      return curr->node;
    }
    curr = curr->next;
    ci++;
  }

  return NULL;
}

void DebugGraph_Free(void) {
  tDebugGraphNode *curr, *next;
  curr = DebugGraphNode_root;

  while (curr) {
    delete curr->node;
    next = curr->next;
    mem_free(curr);
    curr = next;
  }

  DebugGraphNode_root = NULL;

  if (graph_bmp > BAD_BITMAP_HANDLE) {
    bm_FreeBitmap(graph_bmp);
  }
  graph_bmp = -1;
  graph_num_nodes = 0;
}

void DebugGraph_Initialize(void) {
  atexit(DebugGraph_Free);
  DebugGraphNode_root = NULL;
  graph_bmp = -1;
  graph_mask = 0;
  graph_num_nodes = 0;
}

int DebugGraph_Add(int min, int max, const char *name, int flags) {
  tAddDebugGraph data;
  tGraphNode *node = DebugGraph_AddNode();
  if (!node)
    return -1;

  int index = graph_num_nodes - 1;
  data.data_input = DATA_TYPE_INT;
  data.i_max_value = max;
  data.i_min_value = min;
  data.flags = flags;
  node->Initialize(&data);
  node->color = GraphColors[index].color;
  strncpy(node->name, name, MAX_NAME - 1);
  node->name[MAX_NAME - 1] = '\0';
  return index;
}

int DebugGraph_Add(float min, float max, const char *name, int flags) {
  tAddDebugGraph data;
  tGraphNode *node = DebugGraph_AddNode();
  if (!node)
    return -1;

  int index = graph_num_nodes - 1;
  data.data_input = DATA_TYPE_FLOAT;
  data.f_max_value = max;
  data.f_min_value = min;
  data.flags = flags;
  node->Initialize(&data);
  node->color = GraphColors[index].color;
  strncpy(node->name, name, MAX_NAME - 1);
  node->name[MAX_NAME - 1] = '\0';
  return index;
}

void DebugGraph_Enable(int id) {
  int bit = 0x01 << id;
  graph_mask |= bit;
}

void DebugGraph_Disable(int id) {
  int bit = 0x01 << id;
  graph_mask &= ~bit;
}

void DebugGraph_Update(int id, int value) {
  if (id < 0 || id >= graph_num_nodes)
    return;

  tGraphNode *node = DebugGraph_FindNode(id);
  if (!node)
    return;

  node->Update(value);
}

void DebugGraph_Update(int id, float value) {
  if (id < 0 || id >= graph_num_nodes)
    return;

  tGraphNode *node = DebugGraph_FindNode(id);
  if (!node)
    return;

  node->Update(value);
}

#define TOP_L 10
#define TOP_T 50

void DebugGraph_Render(void) {
  if (graph_bmp == -1) {
    // create the bitmap
    graph_bmp = bm_AllocBitmap(64, 64, 0);
    if (graph_bmp > BAD_BITMAP_HANDLE) {
      uint16_t *data = bm_data(graph_bmp, 0);
      for (int i = 0; i < 64 * 64; i++) {
        data[i] = GR_RGB16(0, 0, 0) | OPAQUE_FLAG;
      }
    }
  }
  if (graph_bmp <= BAD_BITMAP_HANDLE)
    return;

  if (graph_mask == 0)
    return;

  rend_SetAlphaType(AT_CONSTANT_TEXTURE);
  rend_SetAlphaValue(240);
  rend_SetLighting(LS_NONE);
  rend_SetColorModel(CM_MONO);
  rend_SetOverlayType(OT_NONE);
  rend_SetWrapType(WT_CLAMP);
  rend_SetFiltering(0);

  rend_DrawScaledBitmap(TOP_L, TOP_T, TOP_L + 256, TOP_T + 64, graph_bmp, 0, 0, 1, 1);

  int bit = 0x01;
  tGraphNode *node;
  bool multi_game = (bool)((Game_mode & GM_MULTI) != 0);
  bool ok_to_render;

  for (int i = 0; i < graph_num_nodes; i++) {
    node = DebugGraph_FindNode(i);
    ok_to_render = true;

    if (node && (graph_mask & bit)) {
      if (multi_game) {
        if (!(node->init_data.flags & DGF_MULTIPLAYER))
          ok_to_render = false;
      } else {
        if (!(node->init_data.flags & DGF_SINGLEPLAYER))
          ok_to_render = false;
      }

      if (ok_to_render)
        node->Render();
    }
    bit = bit << 1;
  }

  rend_SetFiltering(1);
}

////////////////////////////////////////////
void tGraphNode::Initialize(tAddDebugGraph *data) {
  if (!data)
    return;

  memcpy(&init_data, data, sizeof(tAddDebugGraph));
  cur_write_pos = 0;
  first_pos = 0;
  num_items_stored = 0;

  color = GR_RGB(255, 40, 40);
  memset(i_data, 0, sizeof(int) * NUM_FRAMES_TO_DISPLAY);
}

void tGraphNode::Update(int value) {
  ASSERT(init_data.data_input == DATA_TYPE_INT);

  bool multi_game = (bool)((Game_mode & GM_MULTI) != 0);
  if (multi_game) {
    if (!(init_data.flags & DGF_MULTIPLAYER))
      return;
  } else {
    if (!(init_data.flags & DGF_SINGLEPLAYER))
      return;
  }

  if (cur_write_pos != 0 && (cur_write_pos % NUM_FRAMES_TO_DISPLAY) == 0) {
    cur_write_pos = 0;
  }

  if (num_items_stored == NUM_FRAMES_TO_DISPLAY) {
    first_pos++;
    if (first_pos != 0 && (first_pos % NUM_FRAMES_TO_DISPLAY) == 0) {
      first_pos = 0;
    }
  } else {
    num_items_stored++;
  }

  if (value < init_data.i_min_value)
    value = init_data.i_min_value;
  if (value > init_data.i_max_value)
    value = init_data.i_max_value;

  i_data[cur_write_pos] = value;
  cur_write_pos++;
}

void tGraphNode::Update(float value) {
  ASSERT(init_data.data_input == DATA_TYPE_FLOAT);

  bool multi_game = (bool)((Game_mode & GM_MULTI) != 0);
  if (multi_game) {
    if (!(init_data.flags & DGF_MULTIPLAYER))
      return;
  } else {
    if (!(init_data.flags & DGF_SINGLEPLAYER))
      return;
  }

  if (cur_write_pos != 0 && (cur_write_pos % NUM_FRAMES_TO_DISPLAY) == 0) {
    cur_write_pos = 0;
  }

  if (num_items_stored == NUM_FRAMES_TO_DISPLAY) {
    first_pos++;
    if (first_pos != 0 && (first_pos % NUM_FRAMES_TO_DISPLAY) == 0) {
      first_pos = 0;
    }
  } else {
    num_items_stored++;
  }

  if (value < init_data.f_min_value)
    value = init_data.f_min_value;
  if (value > init_data.f_max_value)
    value = init_data.f_max_value;

  f_data[cur_write_pos] = value;

  cur_write_pos++;
}

void GetGraphY(int *y, int value, int max, int min) {
  float range;
  range = float(value - min) / float(max - min);

  *y = TOP_T + 64 - int(range * 64.0);
}

void GetGraphY(int *y, float value, float max, float min) {
  float range;
  range = (value - min) / (max - min);

  *y = TOP_T + 64 - int(range * 64.0);
}

void tGraphNode::Render(void) {
  if (num_items_stored < 2)
    return;
  rend_SetFlatColor(color);

  int last_x, last_y = 0, y;
  int index = first_pos;
  int count = num_items_stored;
  count--;

  switch (init_data.data_input) {
  case DATA_TYPE_FLOAT:
    GetGraphY(&last_y, f_data[index], init_data.f_max_value, init_data.f_min_value);
    break;
  case DATA_TYPE_INT:
    GetGraphY(&last_y, i_data[index], init_data.i_max_value, init_data.i_min_value);
    break;
  };
  index++;
  last_x = TOP_L;

  while (count > 0) {
    if (index != 0 && (index % NUM_FRAMES_TO_DISPLAY) == 0) {
      index = 0;
    }

    switch (init_data.data_input) {
    case DATA_TYPE_FLOAT:
      GetGraphY(&y, f_data[index], init_data.f_max_value, init_data.f_min_value);
      break;
    case DATA_TYPE_INT:
      GetGraphY(&y, i_data[index], init_data.i_max_value, init_data.i_min_value);
      break;
    };

    rend_DrawLine(last_x, last_y, last_x + 1, y);
    count--;
    last_x += 2;
    last_y = y;
    index++;
  }
}

/*
***********************************************************************
*/

void DebugGraph_DisplayOptions(void) {
  if (graph_num_nodes == 0)
    return;

  newuiTiledWindow window;
  newuiSheet *sheet;

  bool **bool_values = NULL;
  bool exit_menu = false;
  tGraphNode *node;

  bool_values = (bool **)mem_malloc(sizeof(bool *) * graph_num_nodes);
  if (!bool_values)
    return;

  window.Create("Debug Graph", 0, 0, 256, 384);
  sheet = window.GetSheet();

  sheet->NewGroup(NULL, 0, 0);
  char buffer[256];
  int bit = 0x01;

  bool multi_game = (bool)((Game_mode & GM_MULTI) != 0);

  for (int i = 0; i < graph_num_nodes; i++) {
    bool_values[i] = NULL;
    node = DebugGraph_FindNode(i);
    if (!node) {
      bit = bit << 1;
      continue;
    }

    // see if we should add it
    if (multi_game) {
      if (!(node->init_data.flags & DGF_MULTIPLAYER)) {
        bit = bit << 1;
        continue;
      }
    } else {
      if (!(node->init_data.flags & DGF_SINGLEPLAYER)) {
        bit = bit << 1;
        continue;
      }
    }

    snprintf(buffer, sizeof(buffer), "%s (%s)", node->name, GraphColors[i].color_name);
    bool_values[i] = sheet->AddLongCheckBox(buffer);
    if (bool_values[i]) {
      *bool_values[i] = (bool)((graph_mask & bit) != 0);
    }

    bit = bit << 1;
  }

  sheet->NewGroup(NULL, 50, 200);
  sheet->AddButton(TXT_OK, UID_OK);

  // process
  window.Open();

  while (!exit_menu) {
    int res = window.DoUI();

    // handle all UI results.
    switch (res) {
    case UID_OK: {
      for (int x = 0; x < graph_num_nodes; x++) {
        if (bool_values[x]) {
          (*bool_values[x]) ? DebugGraph_Enable(x) : DebugGraph_Disable(x);
        }
      }

      exit_menu = true;
    }
    };
  }

  window.Close();
  window.Destroy();

  mem_free(bool_values);
}
