/*
* $Logfile: /DescentIII/Main/czip/HuffmanBasic.cpp $
* $Revision: 2 $
* $Date: 8/27/98 3:26p $
* $Author: Jeff $
*
* Basic Huffman compression functions
*
* $Log: /DescentIII/Main/czip/HuffmanBasic.cpp $
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

int CZip::hb_CompressFile(tVirtualFile *input,BITFILE *output)
{
	ulong *counts;
	tH0Node *nodes;
	tH0Code *codes;
    int root_node;

	int original_pos = VFtell(output->file);

	counts = (ulong *) malloc(256*sizeof(ulong));
	if(!counts)
		return -1;
	if((nodes = (tH0Node *) malloc(514*sizeof(tH0Node)))==NULL){
		free(counts);
		return -1;
	}	
    if((codes = (tH0Code *) malloc( 257*sizeof(tH0Code)))==NULL){
		free(counts);
		free(nodes);
		return -1;
	}

	memset(counts,0,256*sizeof(ulong));
	memset(nodes,0,514*sizeof(tH0Node));
	memset(codes,0,257*sizeof(tH0Code));
	
	hb_count_bytes( input, counts );
	hb_scale_counts( counts, nodes );
	hb_output_counts( output, nodes );
	root_node = hb_build_tree( nodes );
	hb_convert_tree_to_code( nodes, codes, 0, 0, root_node );
	hb_compress_data( input, output, codes );
	free( counts );
	free( nodes );
	free( codes );
	FlushOutputBitFile(output);

	int compsize = VFtell(output->file) - original_pos;
	return compsize;
}

bool CZip::hb_ExpandFile(BITFILE *input,tVirtualFile *output)
{
	tH0Node *nodes;
	int root_node;
	
	if ((nodes = (tH0Node *) malloc( 514*sizeof(tH0Node)))==NULL)
		return false;
	memset(nodes,0,sizeof(tH0Node)*514);
	hb_input_counts( input, nodes );
	root_node = hb_build_tree( nodes );
	hb_expand_data( input, output, nodes, root_node );
	free(nodes);
	return true;
}

void CZip::hb_output_counts(BITFILE *output,tH0Node *nodes)
{
	int first,last,next,i;

	first = 0;
	while ( first < 255 && nodes[ first ].count == 0 )
		first++;

    for ( ; first < 256 ; first = next ) {
		last = first + 1;
		for ( ; ; ) {
			for ( ; last < 256 ; last++ )
				if ( nodes[ last ].count == 0 )
					break;
			last--;
			for ( next = last + 1; next < 256 ; next++ )
				if ( nodes[ next ].count != 0 )
					break;
			if ( next > 255 )
				break;
			if ( ( next - last ) > 3 )
				break;
			last = next;
		}

		if ( VFputc( first, output->file ) != first ){
			//fatal error
		}
		if ( VFputc( last, output->file ) != last ){
			//fatal error
		}
	    for ( i = first ; i <= last ; i++ ) {
			if ( VFputc( nodes[ i ].count, output->file ) !=(int) nodes[ i ].count ){
				//fatal error
			}
		}
	}
	if ( VFputc( 0, output->file ) != 0 ){
	    //fatal error
	}
}

void CZip::hb_input_counts(BITFILE *input,tH0Node *nodes)
{
    int first;
    int last;
    int i;
    int c;

    for ( i = 0 ; i < 256 ; i++ )
		nodes[ i ].count = 0;
    if ( ( first = VFgetc( input->file ) ) == EOF ){
		//	fatal_error( "Error reading byte counts\n" );
	}
    if ( ( last = VFgetc( input->file ) ) == EOF ){
		//fatal_error( "Error reading byte counts\n" );
	}
    for ( ; ; ) {
		for ( i = first ; i <= last ; i++ )
			if ( ( c = VFgetc( input->file ) ) == EOF ){
				//	fatal_error( "Error reading byte counts\n" );
			} else
				nodes[ i ].count = (unsigned int) c;
		if ( ( first = VFgetc( input->file ) ) == EOF ){
			//   fatal_error( "Error reading byte counts\n" );
		}
		if ( first == 0 )
			break;
		if ( ( last = VFgetc( input->file ) ) == EOF ){
			//   fatal_error( "Error reading byte counts\n" );
		}
	}
    nodes[ END_OF_STREAM ].count = 1;
}

void CZip::hb_count_bytes(tVirtualFile *input,ulong *counts)
{
    long input_marker;
    int c;

    input_marker = VFtell( input );
    while ( ( c = VFgetc( input )) != EOF )
		counts[ c ]++;
    VFseek( input, input_marker, SEEK_SET );
}

void CZip::hb_scale_counts(ulong *counts,tH0Node *nodes)
{
    ulong max_count;
    int i;

    max_count = 0;
    for ( i = 0 ; i < 256 ; i++ )
       if ( counts[ i ] > max_count )
		   max_count = counts[ i ];
    if ( max_count == 0 ) {
		counts[ 0 ] = 1;
		max_count = 1;
    }
    max_count = max_count / 255;
    max_count = max_count + 1;
    for ( i = 0 ; i < 256 ; i++ ) {
		nodes[ i ].count = (uint) ( counts[ i ] / max_count );
		if ( nodes[ i ].count == 0 && counts[ i ] != 0 )
			nodes[ i ].count = 1;
    }
    nodes[ END_OF_STREAM ].count = 1;
}

int CZip::hb_build_tree(tH0Node *nodes)
{
    int next_free;
    int i;
    int min_1;
    int min_2;

    nodes[ 513 ].count = 0xffff;
    for ( next_free = END_OF_STREAM + 1 ; ; next_free++ ) {
		min_1 = 513;
		min_2 = 513;
		for ( i = 0 ; i < next_free ; i++ )
			if ( nodes[ i ].count != 0 ) {
				if ( nodes[ i ].count < nodes[ min_1 ].count ) {
					min_2 = min_1;
					min_1 = i;
                } else if ( nodes[ i ].count < nodes[ min_2 ].count )
					min_2 = i;
            }
		if ( min_2 == 513 )
			break;

		nodes[ next_free ].count = nodes[ min_1 ].count+ nodes[ min_2 ].count;
        nodes[ min_1 ].saved_count = nodes[ min_1 ].count;
        nodes[ min_1 ].count = 0;
        nodes[ min_2 ].saved_count =  nodes[ min_2 ].count;
        nodes[ min_2 ].count = 0;
		nodes[ next_free ].child0 = min_1;
		nodes[ next_free ].child1 = min_2;
    }
    next_free--;
    nodes[ next_free ].saved_count = nodes[ next_free ].count;
    return( next_free );
}

void CZip::hb_convert_tree_to_code(tH0Node *nodes,tH0Code *codes,uint code_so_far,int bits,int node)
{
    if ( node <= END_OF_STREAM ) {
		codes[ node ].code = code_so_far;
		codes[ node ].code_bits = bits;
		return;
    }

    code_so_far <<= 1;
    bits++;
    hb_convert_tree_to_code( nodes, codes, code_so_far, bits, nodes[ node ].child0 );
    hb_convert_tree_to_code( nodes, codes, code_so_far | 1,bits, nodes[ node ].child1 );
}

void CZip::hb_compress_data(tVirtualFile *input,BITFILE *output,tH0Code *codes)
{
	int c;
	while((c=VFgetc(input))!=EOF)
		OutputBits(output,(ulong)codes[c].code,codes[c].code_bits);
	OutputBits(output,(ulong)codes[END_OF_STREAM].code,codes[END_OF_STREAM].code_bits);
}

void CZip::hb_expand_data(BITFILE *input,tVirtualFile *output,tH0Node *nodes,int root_node)
{
	int node;
	for(;;){
		node = root_node;
		do{
			if(InputBit(input))
				node = nodes[node].child1;
			else
				node = nodes[node].child0;
		}while(node>END_OF_STREAM);
		if(node==END_OF_STREAM)
			break;
		if((VFputc(node,output))!=node){
			//fatal error
		}
	}
}