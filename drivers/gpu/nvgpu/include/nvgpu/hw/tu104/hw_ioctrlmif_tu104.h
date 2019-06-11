/*
 * Copyright (c) 2018-2019, NVIDIA CORPORATION.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
/*
 * Function naming determines intended use:
 *
 *     <x>_r(void) : Returns the offset for register <x>.
 *
 *     <x>_o(void) : Returns the offset for element <x>.
 *
 *     <x>_w(void) : Returns the word offset for word (4 byte) element <x>.
 *
 *     <x>_<y>_s(void) : Returns size of field <y> of register <x> in bits.
 *
 *     <x>_<y>_f(u32 v) : Returns a value based on 'v' which has been shifted
 *         and masked to place it at field <y> of register <x>.  This value
 *         can be |'d with others to produce a full register value for
 *         register <x>.
 *
 *     <x>_<y>_m(void) : Returns a mask for field <y> of register <x>.  This
 *         value can be ~'d and then &'d to clear the value of field <y> for
 *         register <x>.
 *
 *     <x>_<y>_<z>_f(void) : Returns the constant value <z> after being shifted
 *         to place it at field <y> of register <x>.  This value can be |'d
 *         with others to produce a full register value for <x>.
 *
 *     <x>_<y>_v(u32 r) : Returns the value of field <y> from a full register
 *         <x> value 'r' after being shifted to place its LSB at bit 0.
 *         This value is suitable for direct comparison with other unshifted
 *         values appropriate for use in field <y> of register <x>.
 *
 *     <x>_<y>_<z>_v(void) : Returns the constant value for <z> defined for
 *         field <y> of register <x>.  This value is suitable for direct
 *         comparison with unshifted values appropriate for use in field <y>
 *         of register <x>.
 */
#ifndef NVGPU_HW_IOCTRLMIF_TU104_H
#define NVGPU_HW_IOCTRLMIF_TU104_H

#include <nvgpu/types.h>
#include <nvgpu/safe_ops.h>

static inline u32 ioctrlmif_rx_err_contain_en_0_r(void)
{
	return 0x00000e0cU;
}
static inline u32 ioctrlmif_rx_err_contain_en_0_rxramdataparityerr_f(u32 v)
{
	return (v & 0x1U) << 3U;
}
static inline u32 ioctrlmif_rx_err_contain_en_0_rxramdataparityerr_m(void)
{
	return U32(0x1U) << 3U;
}
static inline u32 ioctrlmif_rx_err_contain_en_0_rxramdataparityerr_v(u32 r)
{
	return (r >> 3U) & 0x1U;
}
static inline u32 ioctrlmif_rx_err_contain_en_0_rxramhdrparityerr_f(u32 v)
{
	return (v & 0x1U) << 4U;
}
static inline u32 ioctrlmif_rx_err_contain_en_0_rxramhdrparityerr_m(void)
{
	return U32(0x1U) << 4U;
}
static inline u32 ioctrlmif_rx_err_contain_en_0_rxramhdrparityerr_v(u32 r)
{
	return (r >> 4U) & 0x1U;
}
static inline u32 ioctrlmif_rx_err_contain_en_0_rxramhdrparityerr__prod_v(void)
{
	return 0x00000001U;
}
static inline u32 ioctrlmif_rx_err_contain_en_0_rxramhdrparityerr__prod_f(void)
{
	return 0x10U;
}
static inline u32 ioctrlmif_rx_err_log_en_0_r(void)
{
	return 0x00000e04U;
}
static inline u32 ioctrlmif_rx_err_log_en_0_rxramdataparityerr_f(u32 v)
{
	return (v & 0x1U) << 3U;
}
static inline u32 ioctrlmif_rx_err_log_en_0_rxramdataparityerr_m(void)
{
	return U32(0x1U) << 3U;
}
static inline u32 ioctrlmif_rx_err_log_en_0_rxramdataparityerr_v(u32 r)
{
	return (r >> 3U) & 0x1U;
}
static inline u32 ioctrlmif_rx_err_log_en_0_rxramhdrparityerr_f(u32 v)
{
	return (v & 0x1U) << 4U;
}
static inline u32 ioctrlmif_rx_err_log_en_0_rxramhdrparityerr_m(void)
{
	return U32(0x1U) << 4U;
}
static inline u32 ioctrlmif_rx_err_log_en_0_rxramhdrparityerr_v(u32 r)
{
	return (r >> 4U) & 0x1U;
}
static inline u32 ioctrlmif_rx_err_report_en_0_r(void)
{
	return 0x00000e08U;
}
static inline u32 ioctrlmif_rx_err_report_en_0_rxramdataparityerr_f(u32 v)
{
	return (v & 0x1U) << 3U;
}
static inline u32 ioctrlmif_rx_err_report_en_0_rxramdataparityerr_m(void)
{
	return U32(0x1U) << 3U;
}
static inline u32 ioctrlmif_rx_err_report_en_0_rxramdataparityerr_v(u32 r)
{
	return (r >> 3U) & 0x1U;
}
static inline u32 ioctrlmif_rx_err_report_en_0_rxramhdrparityerr_f(u32 v)
{
	return (v & 0x1U) << 4U;
}
static inline u32 ioctrlmif_rx_err_report_en_0_rxramhdrparityerr_m(void)
{
	return U32(0x1U) << 4U;
}
static inline u32 ioctrlmif_rx_err_report_en_0_rxramhdrparityerr_v(u32 r)
{
	return (r >> 4U) & 0x1U;
}
static inline u32 ioctrlmif_rx_err_status_0_r(void)
{
	return 0x00000e00U;
}
static inline u32 ioctrlmif_rx_err_status_0_rxramdataparityerr_f(u32 v)
{
	return (v & 0x1U) << 3U;
}
static inline u32 ioctrlmif_rx_err_status_0_rxramdataparityerr_m(void)
{
	return U32(0x1U) << 3U;
}
static inline u32 ioctrlmif_rx_err_status_0_rxramdataparityerr_v(u32 r)
{
	return (r >> 3U) & 0x1U;
}
static inline u32 ioctrlmif_rx_err_status_0_rxramhdrparityerr_f(u32 v)
{
	return (v & 0x1U) << 4U;
}
static inline u32 ioctrlmif_rx_err_status_0_rxramhdrparityerr_m(void)
{
	return U32(0x1U) << 4U;
}
static inline u32 ioctrlmif_rx_err_status_0_rxramhdrparityerr_v(u32 r)
{
	return (r >> 4U) & 0x1U;
}
static inline u32 ioctrlmif_rx_err_first_0_r(void)
{
	return 0x00000e14U;
}
static inline u32 ioctrlmif_tx_err_contain_en_0_r(void)
{
	return 0x00000a90U;
}
static inline u32 ioctrlmif_tx_err_contain_en_0_txramdataparityerr_f(u32 v)
{
	return (v & 0x1U) << 0U;
}
static inline u32 ioctrlmif_tx_err_contain_en_0_txramdataparityerr_m(void)
{
	return U32(0x1U) << 0U;
}
static inline u32 ioctrlmif_tx_err_contain_en_0_txramdataparityerr_v(u32 r)
{
	return (r >> 0U) & 0x1U;
}
static inline u32 ioctrlmif_tx_err_contain_en_0_txramdataparityerr__prod_v(void)
{
	return 0x00000001U;
}
static inline u32 ioctrlmif_tx_err_contain_en_0_txramdataparityerr__prod_f(void)
{
	return 0x1U;
}
static inline u32 ioctrlmif_tx_err_contain_en_0_txramhdrparityerr_f(u32 v)
{
	return (v & 0x1U) << 1U;
}
static inline u32 ioctrlmif_tx_err_contain_en_0_txramhdrparityerr_m(void)
{
	return U32(0x1U) << 1U;
}
static inline u32 ioctrlmif_tx_err_contain_en_0_txramhdrparityerr_v(u32 r)
{
	return (r >> 1U) & 0x1U;
}
static inline u32 ioctrlmif_tx_err_contain_en_0_txramhdrparityerr__prod_v(void)
{
	return 0x00000001U;
}
static inline u32 ioctrlmif_tx_err_contain_en_0_txramhdrparityerr__prod_f(void)
{
	return 0x2U;
}
static inline u32 ioctrlmif_tx_err_log_en_0_r(void)
{
	return 0x00000a88U;
}
static inline u32 ioctrlmif_tx_err_log_en_0_txramdataparityerr_f(u32 v)
{
	return (v & 0x1U) << 0U;
}
static inline u32 ioctrlmif_tx_err_log_en_0_txramdataparityerr_m(void)
{
	return U32(0x1U) << 0U;
}
static inline u32 ioctrlmif_tx_err_log_en_0_txramdataparityerr_v(u32 r)
{
	return (r >> 0U) & 0x1U;
}
static inline u32 ioctrlmif_tx_err_log_en_0_txramhdrparityerr_f(u32 v)
{
	return (v & 0x1U) << 1U;
}
static inline u32 ioctrlmif_tx_err_log_en_0_txramhdrparityerr_m(void)
{
	return U32(0x1U) << 1U;
}
static inline u32 ioctrlmif_tx_err_log_en_0_txramhdrparityerr_v(u32 r)
{
	return (r >> 1U) & 0x1U;
}
static inline u32 ioctrlmif_tx_err_report_en_0_r(void)
{
	return 0x00000e08U;
}
static inline u32 ioctrlmif_tx_err_report_en_0_txramdataparityerr_f(u32 v)
{
	return (v & 0x1U) << 0U;
}
static inline u32 ioctrlmif_tx_err_report_en_0_txramdataparityerr_m(void)
{
	return U32(0x1U) << 0U;
}
static inline u32 ioctrlmif_tx_err_report_en_0_txramdataparityerr_v(u32 r)
{
	return (r >> 0U) & 0x1U;
}
static inline u32 ioctrlmif_tx_err_report_en_0_txramhdrparityerr_f(u32 v)
{
	return (v & 0x1U) << 1U;
}
static inline u32 ioctrlmif_tx_err_report_en_0_txramhdrparityerr_m(void)
{
	return U32(0x1U) << 1U;
}
static inline u32 ioctrlmif_tx_err_report_en_0_txramhdrparityerr_v(u32 r)
{
	return (r >> 1U) & 0x1U;
}
static inline u32 ioctrlmif_tx_err_status_0_r(void)
{
	return 0x00000a84U;
}
static inline u32 ioctrlmif_tx_err_status_0_txramdataparityerr_f(u32 v)
{
	return (v & 0x1U) << 0U;
}
static inline u32 ioctrlmif_tx_err_status_0_txramdataparityerr_m(void)
{
	return U32(0x1U) << 0U;
}
static inline u32 ioctrlmif_tx_err_status_0_txramdataparityerr_v(u32 r)
{
	return (r >> 0U) & 0x1U;
}
static inline u32 ioctrlmif_tx_err_status_0_txramhdrparityerr_f(u32 v)
{
	return (v & 0x1U) << 1U;
}
static inline u32 ioctrlmif_tx_err_status_0_txramhdrparityerr_m(void)
{
	return U32(0x1U) << 1U;
}
static inline u32 ioctrlmif_tx_err_status_0_txramhdrparityerr_v(u32 r)
{
	return (r >> 1U) & 0x1U;
}
static inline u32 ioctrlmif_tx_err_first_0_r(void)
{
	return 0x00000a98U;
}
static inline u32 ioctrlmif_tx_ctrl_buffer_ready_r(void)
{
	return 0x00000a7cU;
}
static inline u32 ioctrlmif_rx_ctrl_buffer_ready_r(void)
{
	return 0x00000dfcU;
}
#endif