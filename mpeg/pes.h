/*****************************************************************************
 * pes.h: ISO/IEC 13818-1 Packetized Elementary Stream
 *****************************************************************************
 * Copyright (C) 2010 VideoLAN
 * $Id$
 *
 * Authors: Christophe Massiot <massiot@via.ecp.fr>
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details.
 *****************************************************************************/

/*
 * Normative references:
 *  - ISO/IEC 13818-1:2007(E) (MPEG-2 systems)
 */

#ifndef __BITSTREAM_MPEG_PES_H__
#define __BITSTREAM_MPEG_PES_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * PES header
 *****************************************************************************/
#define PES_HEADER_SIZE         6
#define PES_HEADER_SIZE_NOPTS   9
#define PES_HEADER_SIZE_PTS     14
#define PES_HEADER_SIZE_PTSDTS  19

#define PES_STREAM_ID_MIN           0xbc
#define PES_STREAM_ID_PRIVATE_1     0xbd
#define PES_STREAM_ID_PRIVATE_2     0xbf
#define PES_STREAM_ID_AUDIO_MPEG    0xc0
#define PES_STREAM_ID_VIDEO_MPEG    0xe0

static inline void pes_init(uint8_t *p_pes)
{
    p_pes[0] = 0x0;
    p_pes[1] = 0x0;
    p_pes[2] = 0x1;
    p_pes[6] = 0x80;
    p_pes[7] = 0x0;
    p_pes[8] = 0x0;
}

static inline void pes_set_streamid(uint8_t *p_pes, uint8_t i_stream_id)
{
    p_pes[3] = i_stream_id;
}

static inline void pes_set_length(uint8_t *p_pes, uint16_t i_length)
{
    p_pes[4] = i_length >> 8;
    p_pes[5] = i_length & 0xff;
}

static inline void pes_set_headerlength(uint8_t *p_pes, uint8_t i_length)
{
    p_pes[6] = 0x80;
    p_pes[7] = 0x0;
    p_pes[8] = i_length;
    if ( i_length > 0 )
        memset( &p_pes[9], 0xff, i_length ); /* stuffing */
}

static inline uint8_t pes_get_headerlength(const uint8_t *p_pes)
{
    return p_pes[8];
}

static inline void pes_set_dataalignment(uint8_t *p_pes)
{
    p_pes[6] |= 0x4;
}

static inline void pes_set_pts(uint8_t *p_pes, uint64_t i_pts)
{
    p_pes[7] |= 0x80;
    if (p_pes[8] < 5)
        p_pes[8] = 5;
    p_pes[9] = 0x21 | ((i_pts >> 29) & 0xe);
    p_pes[10] = (i_pts >> 22) & 0xff;
    p_pes[11] = 0x1 | ((i_pts >> 14) & 0xfe);
    p_pes[12] = (i_pts >> 7) & 0xff;
    p_pes[13] = 0x1 | ((i_pts << 1) & 0xfe);
}

static inline void pes_set_dts(uint8_t *p_pes, uint64_t i_dts)
{
    p_pes[7] |= 0x40;
    if (p_pes[8] < 10)
        p_pes[8] = 10;
    p_pes[14] = 0x11 | ((i_dts >> 29) & 0xe);
    p_pes[15] = (i_dts >> 22) & 0xff;
    p_pes[16] = 0x1 | ((i_dts >> 14) & 0xfe);
    p_pes[17] = (i_dts >> 7) & 0xff;
    p_pes[18] = 0x1 | ((i_dts << 1) & 0xfe);
}

static inline bool pes_validate(const uint8_t *p_pes)
{
    return (p_pes[0] == 0x0 && p_pes[1] == 0x0 && p_pes[2] == 0x1
             && p_pes[3] >= PES_STREAM_ID_MIN);
}

/*****************************************************************************
 * PES payload
 *****************************************************************************/
static inline uint8_t *pes_payload(uint8_t *p_pes)
{
    return p_pes + PES_HEADER_SIZE + pes_get_headerlength(p_pes);
}

#ifdef __cplusplus
}
#endif

#endif