/*
 * GV11B GPU GR
 *
 * Copyright (c) 2016-2019, NVIDIA CORPORATION.  All rights reserved.
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

#ifndef NVGPU_GR_GV11B_H
#define NVGPU_GR_GV11B_H

struct gk20a;
struct nvgpu_warpstate;
struct gk20a_debug_output;

u32 gr_gv11b_ctxsw_checksum_mismatch_mailbox_val(void);
void gr_gv11b_handle_tpc_sm_ecc_exception(struct gk20a *g,
		u32 gpc, u32 tpc,
		bool *post_event, struct nvgpu_channel *fault_ch,
		u32 *hww_global_esr);
void gr_gv11b_set_alpha_circular_buffer_size(struct gk20a *g, u32 data);
void gr_gv11b_set_circular_buffer_size(struct gk20a *g, u32 data);
int gr_gv11b_dump_gr_status_regs(struct gk20a *g,
			   struct gk20a_debug_output *o);
void gr_gv11b_set_gpc_tpc_mask(struct gk20a *g, u32 gpc_index);
int gr_gv11b_pre_process_sm_exception(struct gk20a *g,
		u32 gpc, u32 tpc, u32 sm, u32 global_esr, u32 warp_esr,
		bool sm_debugger_attached, struct nvgpu_channel *fault_ch,
		bool *early_exit, bool *ignore_debugger);
void gv11b_gr_get_esr_sm_sel(struct gk20a *g, u32 gpc, u32 tpc,
				u32 *esr_sm_sel);
int gv11b_gr_sm_trigger_suspend(struct gk20a *g);
void gv11b_gr_bpt_reg_info(struct gk20a *g, struct nvgpu_warpstate *w_state);
int gv11b_gr_set_sm_debug_mode(struct gk20a *g,
	struct nvgpu_channel *ch, u64 sms, bool enable);
u64 gv11b_gr_get_sm_hww_warp_esr_pc(struct gk20a *g, u32 offset);
int gv11b_gr_record_sm_error_state(struct gk20a *g, u32 gpc, u32 tpc, u32 sm,
		struct nvgpu_channel *fault_ch);
int gv11b_gr_clear_sm_error_state(struct gk20a *g,
		struct nvgpu_channel *ch, u32 sm_id);
void gv11b_gr_set_hww_esr_report_mask(struct gk20a *g);
bool gv11b_gr_sm_debugger_attached(struct gk20a *g);
void gv11b_gr_suspend_single_sm(struct gk20a *g,
		u32 gpc, u32 tpc, u32 sm,
		u32 global_esr_mask, bool check_errors);
void gv11b_gr_suspend_all_sms(struct gk20a *g,
		u32 global_esr_mask, bool check_errors);
void gv11b_gr_resume_single_sm(struct gk20a *g,
		u32 gpc, u32 tpc, u32 sm);
void gv11b_gr_resume_all_sms(struct gk20a *g);
int gv11b_gr_resume_from_pause(struct gk20a *g);
u32 gv11b_gr_get_sm_hww_warp_esr(struct gk20a *g,
			u32 gpc, u32 tpc, u32 sm);
u32 gv11b_gr_get_sm_hww_global_esr(struct gk20a *g,
			u32 gpc, u32 tpc, u32 sm);
u32 gv11b_gr_get_sm_no_lock_down_hww_global_esr_mask(struct gk20a *g);
int gv11b_gr_wait_for_sm_lock_down(struct gk20a *g,
		u32 gpc, u32 tpc, u32 sm,
		u32 global_esr_mask, bool check_errors);
int gv11b_gr_lock_down_sm(struct gk20a *g,
			 u32 gpc, u32 tpc, u32 sm, u32 global_esr_mask,
			 bool check_errors);
void gv11b_gr_clear_sm_hww(struct gk20a *g, u32 gpc, u32 tpc, u32 sm,
				u32 global_esr);
void gv11b_gr_init_ovr_sm_dsm_perf(void);
void gv11b_gr_init_sm_dsm_reg_info(void);
void gv11b_gr_get_sm_dsm_perf_regs(struct gk20a *g,
					  u32 *num_sm_dsm_perf_regs,
					  u32 **sm_dsm_perf_regs,
					  u32 *perf_register_stride);
void gv11b_gr_get_sm_dsm_perf_ctrl_regs(struct gk20a *g,
					       u32 *num_sm_dsm_perf_ctrl_regs,
					       u32 **sm_dsm_perf_ctrl_regs,
					       u32 *ctrl_register_stride);
void gv11b_gr_get_ovr_perf_regs(struct gk20a *g, u32 *num_ovr_perf_regs,
					       u32 **ovr_perf_regs);
void gv11b_gr_access_smpc_reg(struct gk20a *g, u32 quad, u32 offset);
bool gv11b_gr_pri_is_egpc_addr(struct gk20a *g, u32 addr);
bool gv11b_gr_pri_is_etpc_addr(struct gk20a *g, u32 addr);
void gv11b_gr_get_egpc_etpc_num(struct gk20a *g, u32 addr,
			u32 *egpc_num, u32 *etpc_num);
int gv11b_gr_decode_egpc_addr(struct gk20a *g, u32 addr,
	enum ctxsw_addr_type *addr_type, u32 *gpc_num, u32 *tpc_num,
	u32 *broadcast_flags);
void gv11b_gr_egpc_etpc_priv_addr_table(struct gk20a *g, u32 addr,
				u32 gpc_num, u32 tpc_num, u32 broadcast_flags,
				u32 *priv_addr_table, u32 *t);
u32 gv11b_gr_get_egpc_base(struct gk20a *g);
int gr_gv11b_handle_ssync_hww(struct gk20a *g, u32 *ssync_esr);
u32 gv11b_gr_sm_offset(struct gk20a *g, u32 sm);
int gr_gv11b_decode_priv_addr(struct gk20a *g, u32 addr,
	enum ctxsw_addr_type *addr_type,
	u32 *gpc_num, u32 *tpc_num, u32 *ppc_num, u32 *be_num,
	u32 *broadcast_flags);
int gr_gv11b_create_priv_addr_table(struct gk20a *g,
	u32 addr,
	u32 *priv_addr_table,
	u32 *num_registers);
void gr_gv11b_powergate_tpc(struct gk20a *g);
bool gv11b_gr_esr_bpt_pending_events(u32 global_esr,
				enum nvgpu_event_id_type bpt_event);
#endif /* NVGPU_GR_GV11B_H */