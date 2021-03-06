/*
 * H.265 video codec.
 * Copyright (c) 2013 StrukturAG, Dirk Farin, <farin@struktur.de>
 *
 * This file is part of libde265.
 *
 * libde265 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libde265 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libde265.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef DE265_H
#define DE265_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


/* === error codes === */

typedef enum {
  DE265_OK = 0,
  DE265_ERROR_NO_SUCH_FILE,
  DE265_ERROR_NO_STARTCODE,
  DE265_ERROR_EOF,
  DE265_ERROR_COEFFICIENT_OUT_OF_IMAGE_BOUNDS,
  DE265_ERROR_CHECKSUM_MISMATCH,
  DE265_ERROR_CTB_OUTSIDE_IMAGE_AREA
} de265_error;

const char* de265_get_error_text(de265_error err);


/* === image === */

/* The image is currently always 3-channel YCbCr, with 4:2:0 chroma.
   But you may want to check the chroma format anyway for future compatibility.
 */

struct de265_image;

enum de265_chroma {
  de265_chroma_mono,
  de265_chroma_420,  // currently the only used format
  de265_chroma_422,
  de265_chroma_444
};


int de265_get_image_width(const struct de265_image*,int channel);
int de265_get_image_height(const struct de265_image*,int channel);
enum de265_chroma de265_get_chroma_format(const struct de265_image*);
const uint8_t* de265_get_image_plane(const struct de265_image*, int channel, int* out_stride);


/* === decoder === */

typedef struct
{
  // private structure
} de265_decoder_context;


enum de265_param {
  DE265_DECODER_PARAM_BOOL_SEI_CHECK_HASH /* Perform SEI hash check on decoded pictures. */
};



/* Static library initialization. */
void de265_init(void);

/* Get a new decoder context. Must be freed with de265_free_decoder(). */
de265_decoder_context* de265_new_decoder(void);

/* Free decoder context. May only be called once on a context. */
void de265_free_decoder(de265_decoder_context*);


/* Push more data into the decoder, must be raw h265.
   All complete images in the data will be decoded, hence, do not push
   too much data at once to prevent image buffer overflows.
   The end of a picture can only be detected when the succeeding start-code
   is read from the data.
   If you want to flush the data and force decoding of the data so far
   (e.g. at the end of a file), call de265_decode_data() with 'length' zero. */
de265_error de265_decode_data(de265_decoder_context*, uint8_t* data, int length);

/* Return next decoded picture, if there is any. If no complete picture has been
   decoded yet, NULL is returned. You should call de265_release_next_picture() to
   advance to the next picture. */
const struct de265_image* de265_peek_next_picture(de265_decoder_context*); // may return NULL

/* Get next decoded picture and remove this picture from the decoder output queue.
   Returns NULL is there is no decoded picture ready.
   You can use the picture only until you call any other de265_* function. */
const struct de265_image* de265_get_next_picture(de265_decoder_context*); // may return NULL

/* Release the current decoded picture for reuse in the decoder. You should not
   use the data anymore after calling this function. */
void de265_release_next_picture(de265_decoder_context*);


/* Set decoding parameters. */
void de265_set_parameter_bool(de265_decoder_context*, enum de265_param param, int value);

/* Get decoding parameters. */
int  de265_get_parameter_bool(de265_decoder_context*, enum de265_param param);


#ifdef __cplusplus
}
#endif

#endif
