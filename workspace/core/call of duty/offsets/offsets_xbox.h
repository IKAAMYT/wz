#pragma once

#include "../../impl/driverless/day1.h"
#include "../sdk/vectors/vectors.h"

namespace decryptions { 
	
	namespace xbox {

		uintptr_t decrypt_client_info()
		{
			const uint64_t mb = g_vm->m_base_address;
			uint64_t rax = mb, rbx = mb, rcx = mb, rdx = mb, rdi = mb, rsi = mb, r8 = mb, r9 = mb, r10 = mb, r11 = mb, r12 = mb, r13 = mb, r14 = mb, r15 = mb;
			rbx = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xE723F48);
			if (!rbx)
				return rbx;
			rdx = ~g_vm->m_peb; 		//mov rdx, gs:[rax]
			rax = rbx; 		//mov rax, rbx
			rax >>= 0x18; 		//shr rax, 0x18
			rcx = 0; 		//and rcx, 0xFFFFFFFFC0000000
			rax ^= rbx; 		//xor rax, rbx
			rcx = _rotl64(rcx, 0x10); 		//rol rcx, 0x10
			rcx ^= g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F29); 		//xor rcx, [0x0000000009838E9D]
			rbx = rax; 		//mov rbx, rax
			rbx >>= 0x30; 		//shr rbx, 0x30
			rcx = ~rcx; 		//not rcx
			rbx ^= rax; 		//xor rbx, rax
			rax = 0x233F5F4AE79533B1; 		//mov rax, 0x233F5F4AE79533B1
			rbx *= rax; 		//imul rbx, rax
			rax = 0x4FF2ED27F19D575D; 		//mov rax, 0x4FF2ED27F19D575D
			rbx -= rdx; 		//sub rbx, rdx
			rbx += rax; 		//add rbx, rax
			rbx ^= r9; 		//xor rbx, r9
			rbx *= g_vm->read<uintptr_t>(rcx + 0x19); 		//imul rbx, [rcx+0x19]
			return rbx;
		}
		uintptr_t decrypt_client_base(uintptr_t client_info)
		{
			const uint64_t mb = g_vm->m_base_address;
			uint64_t rax = mb, rbx = mb, rcx = mb, rdx = mb, rdi = mb, rsi = mb, r8 = mb, r9 = mb, r10 = mb, r11 = mb, r12 = mb, r13 = mb, r14 = mb, r15 = mb;
			r8 = g_vm->read<uintptr_t>(client_info + 0x22f798);
			if (!r8)
				return r8;
			rbx = g_vm->m_peb; 		//mov rbx, gs:[rax]
			rax = rbx; 		//mov rax, rbx
			rax <<= 0x23; 		//shl rax, 0x23
			rax = _byteswap_uint64(rax); 		//bswap rax
			rax &= 0xF;
			switch (rax) {
			case 0:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F5A); 		//mov r10, [0x00000000099FAA54]
				rax = g_vm->m_base_address; 		//lea rax, [0xFFFFFFFFFC46094A]
				r8 -= rax; 		//sub r8, rax
				rax = r8; 		//mov rax, r8
				rax >>= 0x1E; 		//shr rax, 0x1E
				rax ^= r8; 		//xor rax, r8
				r8 = rax; 		//mov r8, rax
				r8 >>= 0x3C; 		//shr r8, 0x3C
				r8 ^= rax; 		//xor r8, rax
				rax = g_vm->m_base_address; 		//lea rax, [0xFFFFFFFFFC46072E]
				r8 -= rax; 		//sub r8, rax
				rax = r8; 		//mov rax, r8
				rax >>= 0x28; 		//shr rax, 0x28
				r8 ^= rax; 		//xor r8, rax
				rax = 0; 		//and rax, 0xFFFFFFFFC0000000
				rax = _rotl64(rax, 0x10); 		//rol rax, 0x10
				rax ^= r10; 		//xor rax, r10
				rax = ~rax; 		//not rax
				r8 *= g_vm->read<uintptr_t>(rax + 0x9); 		//imul r8, [rax+0x09]
				rax = 0x9CC8E0420ADA280D; 		//mov rax, 0x9CC8E0420ADA280D
				rax *= r8; 		//imul rax, r8
				rax += rbx; 		//add rax, rbx
				r8 = rax; 		//mov r8, rax
				r8 >>= 0x11; 		//shr r8, 0x11
				r8 ^= rax; 		//xor r8, rax
				rax = r8; 		//mov rax, r8
				rax >>= 0x22; 		//shr rax, 0x22
				r8 ^= rax; 		//xor r8, rax
				return r8;
			}
			case 1:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F5A); 		//mov r10, [0x00000000099FA5BD]
				r15 = g_vm->m_base_address + 0x755F7BDD; 		//lea r15, [0x0000000071A58228]
				rax = r8; 		//mov rax, r8
				rax >>= 0x9; 		//shr rax, 0x09
				rax ^= r8; 		//xor rax, r8
				r8 = rax; 		//mov r8, rax
				r8 >>= 0x12; 		//shr r8, 0x12
				r8 ^= rax; 		//xor r8, rax
				rax = r8; 		//mov rax, r8
				rax >>= 0x24; 		//shr rax, 0x24
				r8 ^= rax; 		//xor r8, rax
				r8 ^= rbx; 		//xor r8, rbx
				rcx = 0; 		//and rcx, 0xFFFFFFFFC0000000
				rcx = _rotl64(rcx, 0x10); 		//rol rcx, 0x10
				rcx ^= r10; 		//xor rcx, r10
				rcx = ~rcx; 		//not rcx
				r8 *= g_vm->read<uintptr_t>(rcx + 0x9); 		//imul r8, [rcx+0x09]
				rcx = g_vm->m_base_address; 		//lea rcx, [0xFFFFFFFFFC4602B1]
				rax = rbx; 		//mov rax, rbx
				rax -= rcx; 		//sub rax, rcx
				rax += 0xFFFFFFFF9F0CFAED; 		//add rax, 0xFFFFFFFF9F0CFAED
				r8 += rax; 		//add r8, rax
				rax = 0x40ED86BABDEA8F5B; 		//mov rax, 0x40ED86BABDEA8F5B
				r8 *= rax; 		//imul r8, rax
				rax = 0xA7798517B7F399EA; 		//mov rax, 0xA7798517B7F399EA
				r8 ^= rax; 		//xor r8, rax
				rax = r15; 		//mov rax, r15
				rax = ~rax; 		//not rax
				rax ^= rbx; 		//xor rax, rbx
				r8 += rax; 		//add r8, rax
				rax = 0x459093E765583ADB; 		//mov rax, 0x459093E765583ADB
				r8 *= rax; 		//imul r8, rax
				return r8;
			}
			case 2:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F5A); 		//mov r10, [0x00000000099FA16F]
				r14 = g_vm->m_base_address + 0xAC81; 		//lea r14, [0xFFFFFFFFFC46AE83]
				rax = 0xE03443781C6DB26D; 		//mov rax, 0xE03443781C6DB26D
				r8 *= rax; 		//imul r8, rax
				rax = 0x26676A6627BAC50C; 		//mov rax, 0x26676A6627BAC50C
				r8 -= rax; 		//sub r8, rax
				rax = 0x541ECC7788F37ADE; 		//mov rax, 0x541ECC7788F37ADE
				r8 += rax; 		//add r8, rax
				r8 += r14; 		//add r8, r14
				rax = g_vm->m_base_address + 0x142; 		//lea rax, [0xFFFFFFFFFC45FF02]
				rax = ~rax; 		//not rax
				rcx = rbx; 		//mov rcx, rbx
				rcx = ~rcx; 		//not rcx
				rcx -= rbx; 		//sub rcx, rbx
				rcx += rax; 		//add rcx, rax
				r8 += rcx; 		//add r8, rcx
				rax = r8; 		//mov rax, r8
				rax >>= 0x15; 		//shr rax, 0x15
				r8 ^= rax; 		//xor r8, rax
				rax = r8; 		//mov rax, r8
				rax >>= 0x2A; 		//shr rax, 0x2A
				r8 ^= rax; 		//xor r8, rax
				rax = g_vm->m_base_address; 		//lea rax, [0xFFFFFFFFFC45FD61]
				r8 -= rax; 		//sub r8, rax
				rax = 0; 		//and rax, 0xFFFFFFFFC0000000
				rax = _rotl64(rax, 0x10); 		//rol rax, 0x10
				rax ^= r10; 		//xor rax, r10
				rax = ~rax; 		//not rax
				r8 *= g_vm->read<uintptr_t>(rax + 0x9); 		//imul r8, [rax+0x09]
				return r8;
			}
			case 3:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F5A); 		//mov r10, [0x00000000099F9C92]
				r13 = g_vm->m_base_address + 0xF26D; 		//lea r13, [0xFFFFFFFFFC46EF92]
				r8 ^= rbx; 		//xor r8, rbx
				rcx = 0; 		//and rcx, 0xFFFFFFFFC0000000
				rcx = _rotl64(rcx, 0x10); 		//rol rcx, 0x10
				rcx ^= r10; 		//xor rcx, r10
				rcx = ~rcx; 		//not rcx
				r8 *= g_vm->read<uintptr_t>(rcx + 0x9); 		//imul r8, [rcx+0x09]
				rax = 0x6C5618A3BE4C414; 		//mov rax, 0x6C5618A3BE4C414
				r8 -= rax; 		//sub r8, rax
				rax = 0xE98709096AD185CC; 		//mov rax, 0xE98709096AD185CC
				r8 ^= rax; 		//xor r8, rax
				rax = r8; 		//mov rax, r8
				rax >>= 0xB; 		//shr rax, 0x0B
				rax ^= r8; 		//xor rax, r8
				r8 = g_vm->m_base_address + 0x5ED318FB; 		//lea r8, [0x000000005B1914FB]
				rcx = rax; 		//mov rcx, rax
				r8 = ~r8; 		//not r8
				r8 *= rbx; 		//imul r8, rbx
				rcx >>= 0x16; 		//shr rcx, 0x16
				rcx ^= rax; 		//xor rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x2C; 		//shr rax, 0x2C
				r8 ^= rax; 		//xor r8, rax
				r8 ^= rcx; 		//xor r8, rcx
				rax = 0x22A1571E2E749CB; 		//mov rax, 0x22A1571E2E749CB
				r8 *= rax; 		//imul r8, rax
				rax = rbx; 		//mov rax, rbx
				rax *= r13; 		//imul rax, r13
				r8 += rax; 		//add r8, rax
				return r8;
			}
			case 4:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F5A); 		//mov r10, [0x00000000099F9814]
				r15 = g_vm->m_base_address + 0x72D0A311; 		//lea r15, [0x000000006F169BB3]
				rax = 0x54EE9012A77B3C0E; 		//mov rax, 0x54EE9012A77B3C0E
				r8 ^= rax; 		//xor r8, rax
				rax = g_vm->m_base_address; 		//lea rax, [0xFFFFFFFFFC45F451]
				rax += 0x432D; 		//add rax, 0x432D
				rax += rbx; 		//add rax, rbx
				r8 += rax; 		//add r8, rax
				rax = 0xBE6A84FFF3304C3D; 		//mov rax, 0xBE6A84FFF3304C3D
				r8 *= rax; 		//imul r8, rax
				rax = r8; 		//mov rax, r8
				r8 >>= 0x12; 		//shr r8, 0x12
				r8 ^= rax; 		//xor r8, rax
				rax = r8; 		//mov rax, r8
				rax >>= 0x24; 		//shr rax, 0x24
				rax ^= r8; 		//xor rax, r8
				r8 = rax; 		//mov r8, rax
				r8 >>= 0x7; 		//shr r8, 0x07
				r8 ^= rax; 		//xor r8, rax
				rax = r8; 		//mov rax, r8
				rax >>= 0xE; 		//shr rax, 0x0E
				r8 ^= rax; 		//xor r8, rax
				rax = r8; 		//mov rax, r8
				rax >>= 0x1C; 		//shr rax, 0x1C
				rax ^= r8; 		//xor rax, r8
				r8 = rax; 		//mov r8, rax
				r8 >>= 0x38; 		//shr r8, 0x38
				r8 ^= rax; 		//xor r8, rax
				rax = 0; 		//and rax, 0xFFFFFFFFC0000000
				rax = _rotl64(rax, 0x10); 		//rol rax, 0x10
				rax ^= r10; 		//xor rax, r10
				rax = ~rax; 		//not rax
				r8 *= g_vm->read<uintptr_t>(rax + 0x9); 		//imul r8, [rax+0x09]
				rax = rbx; 		//mov rax, rbx
				rax *= r15; 		//imul rax, r15
				r8 -= rax; 		//sub r8, rax
				rax = 0x598660DAA37ACC99; 		//mov rax, 0x598660DAA37ACC99
				r8 ^= rax; 		//xor r8, rax
				return r8;
			}
			case 5:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F5A); 		//mov r10, [0x00000000099F9379]
				rax = 0; 		//and rax, 0xFFFFFFFFC0000000
				rax = _rotl64(rax, 0x10); 		//rol rax, 0x10
				rax ^= r10; 		//xor rax, r10
				rax = ~rax; 		//not rax
				r8 *= g_vm->read<uintptr_t>(rax + 0x9); 		//imul r8, [rax+0x09]
				rax = 0xC088FB236BE68165; 		//mov rax, 0xC088FB236BE68165
				r8 *= rax; 		//imul r8, rax
				rax = r8; 		//mov rax, r8
				r8 >>= 0x5; 		//shr r8, 0x05
				r8 ^= rax; 		//xor r8, rax
				rax = r8; 		//mov rax, r8
				rax >>= 0xA; 		//shr rax, 0x0A
				rax ^= r8; 		//xor rax, r8
				r8 = rax; 		//mov r8, rax
				r8 >>= 0x14; 		//shr r8, 0x14
				r8 ^= rax; 		//xor r8, rax
				rax = r8; 		//mov rax, r8
				rax >>= 0x28; 		//shr rax, 0x28
				r8 ^= rax; 		//xor r8, rax
				rax = r8; 		//mov rax, r8
				rax >>= 0xB; 		//shr rax, 0x0B
				r8 ^= rax; 		//xor r8, rax
				rax = r8; 		//mov rax, r8
				rax >>= 0x16; 		//shr rax, 0x16
				r8 ^= rax; 		//xor r8, rax
				rax = r8; 		//mov rax, r8
				rax >>= 0x2C; 		//shr rax, 0x2C
				r8 ^= rax; 		//xor r8, rax
				rax = 0xF87FD44152069748; 		//mov rax, 0xF87FD44152069748
				r8 ^= rax; 		//xor r8, rax
				rax = g_vm->m_base_address; 		//lea rax, [0xFFFFFFFFFC45F0E6]
				rax += 0x1079; 		//add rax, 0x1079
				rax += rbx; 		//add rax, rbx
				r8 ^= rax; 		//xor r8, rax
				rcx = g_vm->m_base_address; 		//lea rcx, [0xFFFFFFFFFC45F2C7]
				rax = rbx; 		//mov rax, rbx
				rax = ~rax; 		//not rax
				rax -= rcx; 		//sub rax, rcx
				rax += 0xFFFFFFFF968271AB; 		//add rax, 0xFFFFFFFF968271AB
				r8 += rax; 		//add r8, rax
				return r8;
			}
			case 6:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F5A); 		//mov r10, [0x00000000099F8EFB]
				r15 = g_vm->m_base_address + 0x1EE2; 		//lea r15, [0xFFFFFFFFFC460E70]
				rax = 0; 		//and rax, 0xFFFFFFFFC0000000
				rax = _rotl64(rax, 0x10); 		//rol rax, 0x10
				rax ^= r10; 		//xor rax, r10
				rax = ~rax; 		//not rax
				r8 *= g_vm->read<uintptr_t>(rax + 0x9); 		//imul r8, [rax+0x09]
				rcx = 0x30DABF93D6E4FB5; 		//mov rcx, 0x30DABF93D6E4FB5
				r8 ^= rcx; 		//xor r8, rcx
				rax = rbx; 		//mov rax, rbx
				rax ^= r15; 		//xor rax, r15
				r8 -= rax; 		//sub r8, rax
				rax = 0xDB8B0AAFA542904; 		//mov rax, 0xDB8B0AAFA542904
				r8 -= rbx; 		//sub r8, rbx
				r8 -= rax; 		//sub r8, rax
				rax = r8; 		//mov rax, r8
				rax >>= 0x22; 		//shr rax, 0x22
				r8 ^= rax; 		//xor r8, rax
				rax = 0xDF170407BBE28DB5; 		//mov rax, 0xDF170407BBE28DB5
				r8 *= rax; 		//imul r8, rax
				rax = r8; 		//mov rax, r8
				r8 >>= 0x8; 		//shr r8, 0x08
				r8 ^= rax; 		//xor r8, rax
				rax = r8; 		//mov rax, r8
				rax >>= 0x10; 		//shr rax, 0x10
				r8 ^= rax; 		//xor r8, rax
				rax = r8; 		//mov rax, r8
				rax >>= 0x20; 		//shr rax, 0x20
				r8 ^= rax; 		//xor r8, rax
				return r8;
			}
			case 7:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F5A); 		//mov r10, [0x00000000099F8ABA]
				r15 = g_vm->m_base_address + 0xC177; 		//lea r15, [0xFFFFFFFFFC46ACC4]
				rax = 0; 		//and rax, 0xFFFFFFFFC0000000
				rax = _rotl64(rax, 0x10); 		//rol rax, 0x10
				rax ^= r10; 		//xor rax, r10
				rax = ~rax; 		//not rax
				r8 *= g_vm->read<uintptr_t>(rax + 0x9); 		//imul r8, [rax+0x09]
				rax = 0x1C4A7DE2E2F8F68F; 		//mov rax, 0x1C4A7DE2E2F8F68F
				rcx = 0x378CE09B287B2D41; 		//mov rcx, 0x378CE09B287B2D41
				rcx ^= r8; 		//xor rcx, r8
				rcx += rax; 		//add rcx, rax
				r8 = rcx; 		//mov r8, rcx
				r8 >>= 0x23; 		//shr r8, 0x23
				r8 ^= rcx; 		//xor r8, rcx
				rax = rbx + 0x1; 		//lea rax, [rbx+0x01]
				rax *= r15; 		//imul rax, r15
				rax += rbx; 		//add rax, rbx
				r8 += rax; 		//add r8, rax
				rax = 0xEBEA9B8B5714671D; 		//mov rax, 0xEBEA9B8B5714671D
				r8 *= rax; 		//imul r8, rax
				rax = r8; 		//mov rax, r8
				rax >>= 0xE; 		//shr rax, 0x0E
				rax ^= r8; 		//xor rax, r8
				r8 = rax; 		//mov r8, rax
				r8 >>= 0x1C; 		//shr r8, 0x1C
				r8 ^= rax; 		//xor r8, rax
				rax = r8; 		//mov rax, r8
				rax >>= 0x38; 		//shr rax, 0x38
				r8 ^= rax; 		//xor r8, rax
				return r8;
			}
			case 8:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F5A); 		//mov r10, [0x00000000099F8579]
				r13 = g_vm->m_base_address + 0x553; 		//lea r13, [0xFFFFFFFFFC45EB5A]
				r15 = g_vm->m_base_address + 0x88B9; 		//lea r15, [0xFFFFFFFFFC466EAF]
				rax = g_vm->m_base_address; 		//lea rax, [0xFFFFFFFFFC45E420]
				r8 ^= rax; 		//xor r8, rax
				rax = 0x3169FBDB3B875224; 		//mov rax, 0x3169FBDB3B875224
				r8 += rax; 		//add r8, rax
				rax = r15; 		//mov rax, r15
				rax = ~rax; 		//not rax
				rax *= rbx; 		//imul rax, rbx
				r8 ^= rax; 		//xor r8, rax
				r8 ^= rbx; 		//xor r8, rbx
				r8 ^= r13; 		//xor r8, r13
				rax = r8; 		//mov rax, r8
				rax >>= 0x13; 		//shr rax, 0x13
				r8 ^= rax; 		//xor r8, rax
				rax = r8; 		//mov rax, r8
				rax >>= 0x26; 		//shr rax, 0x26
				r8 ^= rax; 		//xor r8, rax
				rcx = 0; 		//and rcx, 0xFFFFFFFFC0000000
				rcx = _rotl64(rcx, 0x10); 		//rol rcx, 0x10
				rax = 0x49665D7F2AFA3F6B; 		//mov rax, 0x49665D7F2AFA3F6B
				r8 *= rax; 		//imul r8, rax
				rcx ^= r10; 		//xor rcx, r10
				rax = g_vm->m_base_address + 0x11D125F7; 		//lea rax, [0x000000000E170950]
				rax = ~rax; 		//not rax
				rcx = ~rcx; 		//not rcx
				rax *= rbx; 		//imul rax, rbx
				r8 += rax; 		//add r8, rax
				r8 *= g_vm->read<uintptr_t>(rcx + 0x9); 		//imul r8, [rcx+0x09]
				return r8;
			}
			case 9:
			{
				r9 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F5A); 		//mov r9, [0x00000000099F813C]
				r11 = g_vm->m_base_address + 0x7C81; 		//lea r11, [0xFFFFFFFFFC465E50]
				rax = rbx; 		//mov rax, rbx
				rax *= r11; 		//imul rax, r11
				r8 -= rax; 		//sub r8, rax
				rax = g_vm->m_base_address; 		//lea rax, [0xFFFFFFFFFC45DEF5]
				r8 -= rax; 		//sub r8, rax
				rax = rbx; 		//mov rax, rbx
				rax -= g_vm->m_base_address; 		//sub rax, [rsp+0x70] -- didn't find trace -> use base[0m
				rax += 0xFFFFFFFFFFFF4D38; 		//add rax, 0xFFFFFFFFFFFF4D38
				r8 += rax; 		//add r8, rax
				rax = 0xB294869EA09D48AA; 		//mov rax, 0xB294869EA09D48AA
				r8 ^= rax; 		//xor r8, rax
				rax = 0xDA6A9700AB4D27FD; 		//mov rax, 0xDA6A9700AB4D27FD
				r8 *= rax; 		//imul r8, rax
				rax = 0; 		//and rax, 0xFFFFFFFFC0000000
				rax = _rotl64(rax, 0x10); 		//rol rax, 0x10
				rax ^= r9; 		//xor rax, r9
				rax = ~rax; 		//not rax
				r8 *= g_vm->read<uintptr_t>(rax + 0x9); 		//imul r8, [rax+0x09]
				rax = 0x38632CDC13FD78A5; 		//mov rax, 0x38632CDC13FD78A5
				r8 += rax; 		//add r8, rax
				rax = r8; 		//mov rax, r8
				rax >>= 0x1D; 		//shr rax, 0x1D
				rax ^= r8; 		//xor rax, r8
				r8 = rax; 		//mov r8, rax
				r8 >>= 0x3A; 		//shr r8, 0x3A
				r8 ^= rax; 		//xor r8, rax
				return r8;
			}
			case 10:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F5A); 		//mov r10, [0x00000000099F7C21]
				r8 += rbx; 		//add r8, rbx
				rax = 0x36164EFD786890C1; 		//mov rax, 0x36164EFD786890C1
				r8 *= rax; 		//imul r8, rax
				rax = 0x6F993F33D7A49418; 		//mov rax, 0x6F993F33D7A49418
				rax += r8; 		//add rax, r8
				r8 = rax; 		//mov r8, rax
				r8 >>= 0x8; 		//shr r8, 0x08
				r8 ^= rax; 		//xor r8, rax
				rcx = r8; 		//mov rcx, r8
				rcx >>= 0x10; 		//shr rcx, 0x10
				rcx ^= r8; 		//xor rcx, r8
				rax = 0; 		//and rax, 0xFFFFFFFFC0000000
				rax = _rotl64(rax, 0x10); 		//rol rax, 0x10
				r8 = rcx; 		//mov r8, rcx
				rax ^= r10; 		//xor rax, r10
				r8 >>= 0x20; 		//shr r8, 0x20
				r8 ^= rcx; 		//xor r8, rcx
				rax = ~rax; 		//not rax
				r8 *= g_vm->read<uintptr_t>(rax + 0x9); 		//imul r8, [rax+0x09]
				rax = 0xE88B55E25B8B057C; 		//mov rax, 0xE88B55E25B8B057C
				r8 ^= rax; 		//xor r8, rax
				rax = r8; 		//mov rax, r8
				rax >>= 0x1A; 		//shr rax, 0x1A
				rax ^= r8; 		//xor rax, r8
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x34; 		//shr rcx, 0x34
				rcx ^= rax; 		//xor rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x4; 		//shr rax, 0x04
				rax ^= rcx; 		//xor rax, rcx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x8; 		//shr rcx, 0x08
				rcx ^= rax; 		//xor rcx, rax
				r8 = rcx; 		//mov r8, rcx
				r8 >>= 0x10; 		//shr r8, 0x10
				r8 ^= rcx; 		//xor r8, rcx
				rax = r8; 		//mov rax, r8
				rax >>= 0x20; 		//shr rax, 0x20
				r8 ^= rax; 		//xor r8, rax
				return r8;
			}
			case 11:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F5A); 		//mov r10, [0x00000000099F77F4]
				rax = 0; 		//and rax, 0xFFFFFFFFC0000000
				rax = _rotl64(rax, 0x10); 		//rol rax, 0x10
				rax ^= r10; 		//xor rax, r10
				rax = ~rax; 		//not rax
				r8 *= g_vm->read<uintptr_t>(rax + 0x9); 		//imul r8, [rax+0x09]
				rax = rbx; 		//mov rax, rbx
				uintptr_t RSP_0xFFFFFFFFFFFFFFB8;
				RSP_0xFFFFFFFFFFFFFFB8 = g_vm->m_base_address + 0xA1FD; 		//lea rax, [0xFFFFFFFFFC467A90] : RBP+0xFFFFFFFFFFFFFFB8
				rax *= RSP_0xFFFFFFFFFFFFFFB8; 		//imul rax, [rbp-0x48]
				r8 += rax; 		//add r8, rax
				rax = r8; 		//mov rax, r8
				rax >>= 0x26; 		//shr rax, 0x26
				r8 ^= rax; 		//xor r8, rax
				rax = r8; 		//mov rax, r8
				rax >>= 0xA; 		//shr rax, 0x0A
				r8 ^= rax; 		//xor r8, rax
				rax = r8; 		//mov rax, r8
				rax >>= 0x14; 		//shr rax, 0x14
				rax ^= r8; 		//xor rax, r8
				r8 = rax; 		//mov r8, rax
				r8 >>= 0x28; 		//shr r8, 0x28
				r8 ^= rax; 		//xor r8, rax
				rax = 0xC6A8E21F37CF3675; 		//mov rax, 0xC6A8E21F37CF3675
				r8 *= rax; 		//imul r8, rax
				rax = g_vm->m_base_address; 		//lea rax, [0xFFFFFFFFFC45D386]
				rax += rbx; 		//add rax, rbx
				r8 -= rax; 		//sub r8, rax
				rax = g_vm->m_base_address; 		//lea rax, [0xFFFFFFFFFC45D4B3]
				r8 ^= rax; 		//xor r8, rax
				return r8;
			}
			case 12:
			{
				r9 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F5A); 		//mov r9, [0x00000000099F72BF]
				rax = 0x5D2901AC55739352; 		//mov rax, 0x5D2901AC55739352
				r8 -= rax; 		//sub r8, rax
				rax = 0; 		//and rax, 0xFFFFFFFFC0000000
				rax = _rotl64(rax, 0x10); 		//rol rax, 0x10
				rax ^= r9; 		//xor rax, r9
				rax = ~rax; 		//not rax
				r8 *= g_vm->read<uintptr_t>(rax + 0x9); 		//imul r8, [rax+0x09]
				rax = g_vm->m_base_address; 		//lea rax, [0xFFFFFFFFFC45CEB1]
				r8 += rax; 		//add r8, rax
				rax = 0x156D71AB28FBFAFF; 		//mov rax, 0x156D71AB28FBFAFF
				r8 *= rax; 		//imul r8, rax
				rax = r8; 		//mov rax, r8
				r8 >>= 0x27; 		//shr r8, 0x27
				r8 ^= rax; 		//xor r8, rax
				r8 -= rbx; 		//sub r8, rbx
				rax = r8; 		//mov rax, r8
				rax >>= 0x17; 		//shr rax, 0x17
				r8 ^= rax; 		//xor r8, rax
				rax = r8; 		//mov rax, r8
				rax >>= 0x2E; 		//shr rax, 0x2E
				r8 ^= rax; 		//xor r8, rax
				rax = g_vm->m_base_address; 		//lea rax, [0xFFFFFFFFFC45CF33]
				r8 ^= rax; 		//xor r8, rax
				return r8;
			}
			case 13:
			{
				r11 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F5A); 		//mov r11, [0x00000000099F6D76]
				rax = r8; 		//mov rax, r8
				rax >>= 0x1F; 		//shr rax, 0x1F
				r8 ^= rax; 		//xor r8, rax
				rcx = r8; 		//mov rcx, r8
				rax = g_vm->m_base_address; 		//lea rax, [0xFFFFFFFFFC45CA1F]
				rcx >>= 0x3E; 		//shr rcx, 0x3E
				rcx ^= r8; 		//xor rcx, r8
				rdx = 0; 		//and rdx, 0xFFFFFFFFC0000000
				rdx = _rotl64(rdx, 0x10); 		//rol rdx, 0x10
				r8 = rbx; 		//mov r8, rbx
				r8 = ~r8; 		//not r8
				rdx ^= r11; 		//xor rdx, r11
				r8 += rcx; 		//add r8, rcx
				rdx = ~rdx; 		//not rdx
				r8 -= rax; 		//sub r8, rax
				r8 -= 0x6929AFAC; 		//sub r8, 0x6929AFAC
				r8 *= g_vm->read<uintptr_t>(rdx + 0x9); 		//imul r8, [rdx+0x09]
				rax = r8; 		//mov rax, r8
				rax >>= 0x18; 		//shr rax, 0x18
				r8 ^= rax; 		//xor r8, rax
				rax = r8; 		//mov rax, r8
				rax >>= 0x30; 		//shr rax, 0x30
				r8 ^= rax; 		//xor r8, rax
				rax = 0x69651B1AF033619B; 		//mov rax, 0x69651B1AF033619B
				r8 += rbx; 		//add r8, rbx
				r8 *= rax; 		//imul r8, rax
				rax = 0x29BBD1B30DFD9417; 		//mov rax, 0x29BBD1B30DFD9417
				r8 *= rax; 		//imul r8, rax
				rax = 0xA7B8F15C4FABBB6C; 		//mov rax, 0xA7B8F15C4FABBB6C
				r8 ^= rax; 		//xor r8, rax
				return r8;
			}
			case 14:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F5A); 		//mov r10, [0x00000000099F68FA]
				r8 += rbx; 		//add r8, rbx
				rax = g_vm->m_base_address + 0x8D0; 		//lea rax, [0xFFFFFFFFFC45D0D1]
				rax -= rbx; 		//sub rax, rbx
				r8 += rax; 		//add r8, rax
				rax = 0xBC0AAA7E98B1663A; 		//mov rax, 0xBC0AAA7E98B1663A
				r8 ^= rax; 		//xor r8, rax
				rax = 0x54D1F9305B205B45; 		//mov rax, 0x54D1F9305B205B45
				r8 *= rax; 		//imul r8, rax
				rcx = r8; 		//mov rcx, r8
				rcx >>= 0xA; 		//shr rcx, 0x0A
				rcx ^= r8; 		//xor rcx, r8
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x14; 		//shr rax, 0x14
				rax ^= rcx; 		//xor rax, rcx
				r8 = rax; 		//mov r8, rax
				r8 >>= 0x28; 		//shr r8, 0x28
				r8 ^= rax; 		//xor r8, rax
				rax = r8; 		//mov rax, r8
				rax >>= 0x12; 		//shr rax, 0x12
				rax ^= r8; 		//xor rax, r8
				r8 = rax; 		//mov r8, rax
				r8 >>= 0x24; 		//shr r8, 0x24
				r8 ^= rax; 		//xor r8, rax
				rax = 0xFFFFFFFFDE23E20A; 		//mov rax, 0xFFFFFFFFDE23E20A
				rax -= rbx; 		//sub rax, rbx
				rax -= g_vm->m_base_address; 		//sub rax, [rsp+0x70] -- didn't find trace -> use base[0m
				r8 += rax; 		//add r8, rax
				rax = 0; 		//and rax, 0xFFFFFFFFC0000000
				rax = _rotl64(rax, 0x10); 		//rol rax, 0x10
				rax ^= r10; 		//xor rax, r10
				rax = ~rax; 		//not rax
				r8 *= g_vm->read<uintptr_t>(rax + 0x9); 		//imul r8, [rax+0x09]
				return r8;
			}
			case 15:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F5A); 		//mov r10, [0x00000000099F6476]
				r15 = g_vm->m_base_address + 0x76BB; 		//lea r15, [0xFFFFFFFFFC463BC4]
				rax = 0; 		//and rax, 0xFFFFFFFFC0000000
				rax = _rotl64(rax, 0x10); 		//rol rax, 0x10
				rax ^= r10; 		//xor rax, r10
				rax = ~rax; 		//not rax
				rax = g_vm->read<uintptr_t>(rax + 0x9); 		//mov rax, [rax+0x09]
				uintptr_t RSP_0xFFFFFFFFFFFFFFB0;
				RSP_0xFFFFFFFFFFFFFFB0 = 0x3A27415DA31CA989; 		//mov rax, 0x3A27415DA31CA989 : RBP+0xFFFFFFFFFFFFFFB0
				rax *= RSP_0xFFFFFFFFFFFFFFB0; 		//imul rax, [rbp-0x50]
				r8 *= rax; 		//imul r8, rax
				rax = 0x6F6A3BE0CADE4A54; 		//mov rax, 0x6F6A3BE0CADE4A54
				r8 -= rax; 		//sub r8, rax
				r8 -= rbx; 		//sub r8, rbx
				rcx = r8; 		//mov rcx, r8
				rcx >>= 0x13; 		//shr rcx, 0x13
				rcx ^= r8; 		//xor rcx, r8
				r8 = rbx; 		//mov r8, rbx
				r8 = ~r8; 		//not r8
				rax = r15; 		//mov rax, r15
				rax = ~rax; 		//not rax
				r8 *= rax; 		//imul r8, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x26; 		//shr rax, 0x26
				rax ^= rcx; 		//xor rax, rcx
				r8 += rax; 		//add r8, rax
				rax = r8; 		//mov rax, r8
				rax >>= 0x28; 		//shr rax, 0x28
				r8 ^= rax; 		//xor r8, rax
				rax = 0x3224CE0A9BEB6A6E; 		//mov rax, 0x3224CE0A9BEB6A6E
				r8 -= rax; 		//sub r8, rax
				return r8;
			}
			}
		}
		uintptr_t GetCmdTextArray()
		{
			const uint64_t mb = g_vm->m_base_address;
			uint64_t rax = mb, rbx = mb, rcx = mb, rdx = mb, rdi = mb, rsi = mb, r8 = mb, r9 = mb, r10 = mb, r11 = mb, r12 = mb, r13 = mb, r14 = mb, r15 = mb;
			rcx = g_vm->read<uintptr_t>(g_vm->m_base_address + 0x12E93BD8);
			if (!rcx)
				return rcx;
			r11 = g_vm->m_peb; 		//mov r11, gs:[rdx]
			// stack save: [rsp+0xb0] = r12; 		//mov [rsp+0xB0], r12
			rdx = r11; 		//mov rdx, r11
			rdx = _rotl64(rdx, 0x22); 		//rol rdx, 0x22
			rdx &= 0xF;
			switch (rdx) {
			case 0:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599FD9); 		//mov r10, [0x00000000059B8440]
				rbx = g_vm->m_base_address; 		//lea rbx, [0xFFFFFFFFF841E460]
				r15 = g_vm->m_base_address + 0x5E33; 		//lea r15, [0xFFFFFFFFF8424287]
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x1B; 		//shr rax, 0x1B
				rcx ^= rax; 		//xor rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x36; 		//shr rax, 0x36
				rcx ^= rax; 		//xor rcx, rax
				rax = 0xB5E3A5C14A756615; 		//mov rax, 0xB5E3A5C14A756615
				rcx ^= rax; 		//xor rcx, rax
				rcx *= g_vm->read<uintptr_t>(r10 + 0x15); 		//imul rcx, [r10+0x15]
				rax = r11; 		//mov rax, r11
				rax *= r15; 		//imul rax, r15
				rcx += rax; 		//add rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x17; 		//shr rax, 0x17
				rcx ^= rax; 		//xor rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x2E; 		//shr rax, 0x2E
				rcx ^= rax; 		//xor rcx, rax
				rcx += rbx; 		//add rcx, rbx
				rax = 0x451CDD50ED47051D; 		//mov rax, 0x451CDD50ED47051D
				rcx *= rax; 		//imul rcx, rax
				rax = rcx; 		//mov rax, rcx
				rcx >>= 0x27; 		//shr rcx, 0x27
				rcx ^= rax; 		//xor rcx, rax
				return rcx;
			}
			case 1:
			{
				r9 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599FD9); 		//mov r9, [0x00000000059B7F48]
				rbx = g_vm->m_base_address; 		//lea rbx, [0xFFFFFFFFF841DF68]
				rcx ^= rbx; 		//xor rcx, rbx
				rcx *= g_vm->read<uintptr_t>(r9 + 0x15); 		//imul rcx, [r9+0x15]
				rax = 0x3F5FF8759C0450AD; 		//mov rax, 0x3F5FF8759C0450AD
				rcx *= rax; 		//imul rcx, rax
				rax = 0x49224897BA23AD81; 		//mov rax, 0x49224897BA23AD81
				rcx *= rax; 		//imul rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x26; 		//shr rax, 0x26
				rcx ^= rax; 		//xor rcx, rax
				rax = 0x9308B88C835A243; 		//mov rax, 0x9308B88C835A243
				rcx -= rax; 		//sub rcx, rax
				return rcx;
			}
			case 2:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599FD9); 		//mov r10, [0x00000000059B7B20]
				rbx = g_vm->m_base_address; 		//lea rbx, [0xFFFFFFFFF841DB40]
				r12 = g_vm->m_base_address + 0xC499; 		//lea r12, [0xFFFFFFFFF8429FC8]
				rcx ^= r11; 		//xor rcx, r11
				rcx ^= r12; 		//xor rcx, r12
				rcx += r11; 		//add rcx, r11
				rax = 0xDD209D02F3077017; 		//mov rax, 0xDD209D02F3077017
				rcx *= rax; 		//imul rcx, rax
				rcx *= g_vm->read<uintptr_t>(r10 + 0x15); 		//imul rcx, [r10+0x15]
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x12; 		//shr rax, 0x12
				rcx ^= rax; 		//xor rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x24; 		//shr rax, 0x24
				rcx ^= rax; 		//xor rcx, rax
				rax = 0x7B3DE336F351C129; 		//mov rax, 0x7B3DE336F351C129
				rcx -= rax; 		//sub rcx, rax
				rcx ^= rbx; 		//xor rcx, rbx
				return rcx;
			}
			case 3:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599FD9); 		//mov r10, [0x00000000059B7781]
				rbx = g_vm->m_base_address; 		//lea rbx, [0xFFFFFFFFF841D796]
				rax = 0x292A737D3A8A003D; 		//mov rax, 0x292A737D3A8A003D
				rcx *= rax; 		//imul rcx, rax
				rcx ^= r11; 		//xor rcx, r11
				rcx ^= r11; 		//xor rcx, r11
				rcx -= rbx; 		//sub rcx, rbx
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x19; 		//shr rax, 0x19
				rcx ^= rax; 		//xor rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x32; 		//shr rax, 0x32
				rcx ^= rax; 		//xor rcx, rax
				rcx *= g_vm->read<uintptr_t>(r10 + 0x15); 		//imul rcx, [r10+0x15]
				rdx = r11; 		//mov rdx, r11
				rdx = ~rdx; 		//not rdx
				rax = g_vm->m_base_address + 0x4F2A; 		//lea rax, [0xFFFFFFFFF842230A]
				rcx += rax; 		//add rcx, rax
				rcx += rdx; 		//add rcx, rdx
				rax = 0x1D53DC9394D6C68A; 		//mov rax, 0x1D53DC9394D6C68A
				rcx += rax; 		//add rcx, rax
				return rcx;
			}
			case 4:
			{
				r9 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599FD9); 		//mov r9, [0x00000000059B7355]
				rbx = g_vm->m_base_address; 		//lea rbx, [0xFFFFFFFFF841D375]
				rdi = g_vm->m_base_address + 0x46C0CD17; 		//lea rdi, [0x000000003F02A080]
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x10; 		//shr rax, 0x10
				rcx ^= rax; 		//xor rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x20; 		//shr rax, 0x20
				rcx ^= rax; 		//xor rcx, rax
				rcx ^= r11; 		//xor rcx, r11
				rcx ^= rdi; 		//xor rcx, rdi
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x17; 		//shr rax, 0x17
				rax ^= rcx; 		//xor rax, rcx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x2E; 		//shr rcx, 0x2E
				rcx ^= rax; 		//xor rcx, rax
				rax = 0x2A27FA74C256E0E7; 		//mov rax, 0x2A27FA74C256E0E7
				rcx *= rax; 		//imul rcx, rax
				rcx += rbx; 		//add rcx, rbx
				rcx *= g_vm->read<uintptr_t>(r9 + 0x15); 		//imul rcx, [r9+0x15]
				rax = 0xE889A0595E16BFDB; 		//mov rax, 0xE889A0595E16BFDB
				rcx *= rax; 		//imul rcx, rax
				return rcx;
			}
			case 5:
			{
				r9 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599FD9); 		//mov r9, [0x00000000059B6E35]
				rdi = g_vm->m_base_address + 0x6CA3; 		//lea rdi, [0xFFFFFFFFF8423AF8]
				rax = 0x4694415E362AF97D; 		//mov rax, 0x4694415E362AF97D
				rcx *= rax; 		//imul rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0xE; 		//shr rax, 0x0E
				rcx ^= rax; 		//xor rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x1C; 		//shr rax, 0x1C
				rax ^= rcx; 		//xor rax, rcx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x38; 		//shr rcx, 0x38
				rcx ^= rax; 		//xor rcx, rax
				rax = 0x2A796B075D8038A1; 		//mov rax, 0x2A796B075D8038A1
				rcx ^= rax; 		//xor rcx, rax
				rcx += r11; 		//add rcx, r11
				rax = 0x3212ABD7EB1A3183; 		//mov rax, 0x3212ABD7EB1A3183
				rcx += rax; 		//add rcx, rax
				rcx *= g_vm->read<uintptr_t>(r9 + 0x15); 		//imul rcx, [r9+0x15]
				rax = r11; 		//mov rax, r11
				rax *= rdi; 		//imul rax, rdi
				rcx -= rax; 		//sub rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x18; 		//shr rax, 0x18
				rcx ^= rax; 		//xor rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x30; 		//shr rax, 0x30
				rcx ^= rax; 		//xor rcx, rax
				return rcx;
			}
			case 6:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599FD9); 		//mov r10, [0x00000000059B6963]
				rbx = g_vm->m_base_address; 		//lea rbx, [0xFFFFFFFFF841C983]
				r12 = g_vm->m_base_address + 0x2407; 		//lea r12, [0xFFFFFFFFF841ED7E]
				r13 = g_vm->m_base_address + 0xD54B; 		//lea r13, [0xFFFFFFFFF8429EB7]
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x8; 		//shr rax, 0x08
				rcx ^= rax; 		//xor rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x10; 		//shr rax, 0x10
				rax ^= rcx; 		//xor rax, rcx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x20; 		//shr rcx, 0x20
				rcx ^= rax; 		//xor rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0xF; 		//shr rax, 0x0F
				rax ^= rcx; 		//xor rax, rcx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x1E; 		//shr rcx, 0x1E
				rcx ^= rax; 		//xor rcx, rax
				rdx = rcx; 		//mov rdx, rcx
				rdx >>= 0x3C; 		//shr rdx, 0x3C
				rax = r12; 		//mov rax, r12
				rax = ~rax; 		//not rax
				rdx ^= rax; 		//xor rdx, rax
				rdx ^= r11; 		//xor rdx, r11
				rcx ^= rdx; 		//xor rcx, rdx
				rax = rbx + 0x86d9; 		//lea rax, [rbx+0x86D9]
				rax += r11; 		//add rax, r11
				rcx ^= rax; 		//xor rcx, rax
				rax = 0x394D31FBFC54D5C7; 		//mov rax, 0x394D31FBFC54D5C7
				rcx *= rax; 		//imul rcx, rax
				rax = r13; 		//mov rax, r13
				rax = ~rax; 		//not rax
				rax *= r11; 		//imul rax, r11
				rcx ^= rax; 		//xor rcx, rax
				rax = 0xF248AD144683687F; 		//mov rax, 0xF248AD144683687F
				rcx *= rax; 		//imul rcx, rax
				rcx *= g_vm->read<uintptr_t>(r10 + 0x15); 		//imul rcx, [r10+0x15]
				return rcx;
			}
			case 7:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599FD9); 		//mov r10, [0x00000000059B64AF]
				rdi = g_vm->m_base_address + 0xF2B7; 		//lea rdi, [0xFFFFFFFFF842B786]
				r15 = g_vm->m_base_address + 0x727BCC8E; 		//lea r15, [0x000000006ABD9151]
				rcx += r15; 		//add rcx, r15
				rdx = r11; 		//mov rdx, r11
				rdx = ~rdx; 		//not rdx
				rax = rdx; 		//mov rax, rdx
				rax ^= rdi; 		//xor rax, rdi
				rdx -= rax; 		//sub rdx, rax
				rcx += rdx; 		//add rcx, rdx
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x18; 		//shr rax, 0x18
				rcx ^= rax; 		//xor rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x30; 		//shr rax, 0x30
				rcx ^= rax; 		//xor rcx, rax
				rax = 0xD4748743E26DDED6; 		//mov rax, 0xD4748743E26DDED6
				rax += rcx; 		//add rax, rcx
				rdx = rax; 		//mov rdx, rax
				rdx >>= 0x17; 		//shr rdx, 0x17
				rdx ^= rax; 		//xor rdx, rax
				rcx = rdx; 		//mov rcx, rdx
				rcx >>= 0x2E; 		//shr rcx, 0x2E
				rcx ^= rdx; 		//xor rcx, rdx
				rax = 0x49781D9754795626; 		//mov rax, 0x49781D9754795626
				rcx ^= rax; 		//xor rcx, rax
				rax = 0xE1A998C5EA9145DD; 		//mov rax, 0xE1A998C5EA9145DD
				rcx *= rax; 		//imul rcx, rax
				rcx *= g_vm->read<uintptr_t>(r10 + 0x15); 		//imul rcx, [r10+0x15]
				return rcx;
			}
			case 8:
			{
				r9 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599FD9); 		//mov r9, [0x00000000059B6090]
				rbx = g_vm->m_base_address; 		//lea rbx, [0xFFFFFFFFF841C0B0]
				rcx ^= r11; 		//xor rcx, r11
				rax = 0xD0FA4725C1F6A4DB; 		//mov rax, 0xD0FA4725C1F6A4DB
				rcx *= rax; 		//imul rcx, rax
				rcx += rbx; 		//add rcx, rbx
				rcx *= g_vm->read<uintptr_t>(r9 + 0x15); 		//imul rcx, [r9+0x15]
				rax = 0x2280D092AFB637A3; 		//mov rax, 0x2280D092AFB637A3
				rcx ^= rax; 		//xor rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x21; 		//shr rax, 0x21
				rax ^= rcx; 		//xor rax, rcx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x1D; 		//shr rcx, 0x1D
				rcx ^= rax; 		//xor rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x3A; 		//shr rax, 0x3A
				rcx ^= rax; 		//xor rcx, rax
				return rcx;
			}
			case 9:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599FD9); 		//mov r10, [0x00000000059B5C60]
				rbx = g_vm->m_base_address; 		//lea rbx, [0xFFFFFFFFF841BC80]
				rdi = g_vm->m_base_address + 0xF805; 		//lea rdi, [0xFFFFFFFFF842B474]
				rax = r11; 		//mov rax, r11
				rax *= rdi; 		//imul rax, rdi
				rcx -= rax; 		//sub rcx, rax
				rcx += r11; 		//add rcx, r11
				rax = g_vm->m_base_address + 0x34E2; 		//lea rax, [0xFFFFFFFFF841F0D3]
				rcx += rax; 		//add rcx, rax
				rax = 0xD670AE07CD87C44B; 		//mov rax, 0xD670AE07CD87C44B
				rcx *= rax; 		//imul rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x11; 		//shr rax, 0x11
				rax ^= rcx; 		//xor rax, rcx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x22; 		//shr rcx, 0x22
				rcx ^= rax; 		//xor rcx, rax
				rax = g_vm->read<uintptr_t>(r10 + 0x15); 		//mov rax, [r10+0x15]
				rdx = 0x7DF246B4B47C04D; 		//mov rdx, 0x7DF246B4B47C04D
				rax *= rdx; 		//imul rax, rdx
				rcx *= rax; 		//imul rcx, rax
				rax = r11; 		//mov rax, r11
				rax -= rbx; 		//sub rax, rbx
				rax -= 0x3EA4E863; 		//sub rax, 0x3EA4E863
				rcx ^= rax; 		//xor rcx, rax
				rax = 0x6EAEB149F1BB523E; 		//mov rax, 0x6EAEB149F1BB523E
				rcx ^= rax; 		//xor rcx, rax
				return rcx;
			}
			case 10:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599FD9); 		//mov r10, [0x00000000059B5910]
				rbx = g_vm->m_base_address; 		//lea rbx, [0xFFFFFFFFF841B930]
				rcx += r11; 		//add rcx, r11
				rcx ^= rbx; 		//xor rcx, rbx
				rcx -= rbx; 		//sub rcx, rbx
				rax = 0xC2C77ADBEA06CC71; 		//mov rax, 0xC2C77ADBEA06CC71
				rcx *= rax; 		//imul rcx, rax
				rax = rcx; 		//mov rax, rcx
				rcx >>= 0x20; 		//shr rcx, 0x20
				rcx ^= rax; 		//xor rcx, rax
				rax = 0x95421665B36BD3E3; 		//mov rax, 0x95421665B36BD3E3
				rax += rcx; 		//add rax, rcx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0xB; 		//shr rcx, 0x0B
				rcx ^= rax; 		//xor rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x16; 		//shr rax, 0x16
				rcx ^= rax; 		//xor rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x2C; 		//shr rax, 0x2C
				rcx ^= rax; 		//xor rcx, rax
				rcx *= g_vm->read<uintptr_t>(r10 + 0x15); 		//imul rcx, [r10+0x15]
				return rcx;
			}
			case 11:
			{
				r9 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599FD9); 		//mov r9, [0x00000000059B5524]
				rbx = g_vm->m_base_address; 		//lea rbx, [0xFFFFFFFFF841B544]
				rax = rcx; 		//mov rax, rcx
				rax >>= 0xA; 		//shr rax, 0x0A
				rcx ^= rax; 		//xor rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x14; 		//shr rax, 0x14
				rax ^= rcx; 		//xor rax, rcx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x28; 		//shr rcx, 0x28
				rcx ^= rax; 		//xor rcx, rax
				rcx -= r11; 		//sub rcx, r11
				rax = rbx + 0x723d2a5a; 		//lea rax, [rbx+0x723D2A5A]
				rax += r11; 		//add rax, r11
				rcx ^= rax; 		//xor rcx, rax
				rax = 0x405646879A90A4B1; 		//mov rax, 0x405646879A90A4B1
				rcx *= rax; 		//imul rcx, rax
				rax = 0x7E1BE77430218FC2; 		//mov rax, 0x7E1BE77430218FC2
				rcx += rax; 		//add rcx, rax
				rax = 0xF28B8C5F9F41196A; 		//mov rax, 0xF28B8C5F9F41196A
				rcx ^= rax; 		//xor rcx, rax
				rcx *= g_vm->read<uintptr_t>(r9 + 0x15); 		//imul rcx, [r9+0x15]
				rax = rcx; 		//mov rax, rcx
				rcx >>= 0xF; 		//shr rcx, 0x0F
				rcx ^= rax; 		//xor rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x1E; 		//shr rax, 0x1E
				rcx ^= rax; 		//xor rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x3C; 		//shr rax, 0x3C
				rcx ^= rax; 		//xor rcx, rax
				return rcx;
			}
			case 12:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599FD9); 		//mov r10, [0x00000000059B506E]
				rbx = g_vm->m_base_address; 		//lea rbx, [0xFFFFFFFFF841B08E]
				r15 = g_vm->m_base_address + 0x339332F9; 		//lea r15, [0x000000002BD4E376]
				rax = r11; 		//mov rax, r11
				rax *= r15; 		//imul rax, r15
				rcx -= rax; 		//sub rcx, rax
				rax = 0x3038E6AB6FEAABA9; 		//mov rax, 0x3038E6AB6FEAABA9
				rcx *= rax; 		//imul rcx, rax
				rcx ^= r11; 		//xor rcx, r11
				rcx += r11; 		//add rcx, r11
				rcx *= g_vm->read<uintptr_t>(r10 + 0x15); 		//imul rcx, [r10+0x15]
				rcx -= rbx; 		//sub rcx, rbx
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x25; 		//shr rax, 0x25
				rcx ^= rax; 		//xor rcx, rax
				rcx += r11; 		//add rcx, r11
				return rcx;
			}
			case 13:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599FD9); 		//mov r10, [0x00000000059B4CCC]
				rbx = g_vm->m_base_address; 		//lea rbx, [0xFFFFFFFFF841ACEC]
				rax = g_vm->m_base_address + 0x45FE7DB0; 		//lea rax, [0x000000003E402966]
				rax -= r11; 		//sub rax, r11
				rcx ^= rax; 		//xor rcx, rax
				rax = rbx + 0x44c85679; 		//lea rax, [rbx+0x44C85679]
				rax += r11; 		//add rax, r11
				rcx += rax; 		//add rcx, rax
				rcx *= g_vm->read<uintptr_t>(r10 + 0x15); 		//imul rcx, [r10+0x15]
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x1B; 		//shr rax, 0x1B
				rcx ^= rax; 		//xor rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x36; 		//shr rax, 0x36
				rcx ^= rax; 		//xor rcx, rax
				rax = 0xDC51C6CA5ECBE269; 		//mov rax, 0xDC51C6CA5ECBE269
				rcx *= rax; 		//imul rcx, rax
				rax = rbx * 0xFFFFFFFFFFFFFFFE; 		//imul rax, rbx, 0xFFFFFFFFFFFFFFFE
				rcx += rax; 		//add rcx, rax
				rax = 0xA8C2E47322E167D3; 		//mov rax, 0xA8C2E47322E167D3
				rcx -= r11; 		//sub rcx, r11
				rcx -= 0x6842; 		//sub rcx, 0x6842
				rcx ^= rax; 		//xor rcx, rax
				return rcx;
			}
			case 14:
			{
				r9 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599FD9); 		//mov r9, [0x00000000059B48C6]
				rbx = g_vm->m_base_address; 		//lea rbx, [0xFFFFFFFFF841A8E6]
				rcx ^= rbx; 		//xor rcx, rbx
				rax = 0xC2092D064BD7621B; 		//mov rax, 0xC2092D064BD7621B
				rcx *= rax; 		//imul rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0xA; 		//shr rax, 0x0A
				rax ^= rcx; 		//xor rax, rcx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x14; 		//shr rcx, 0x14
				rcx ^= rax; 		//xor rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x28; 		//shr rax, 0x28
				rcx ^= rax; 		//xor rcx, rax
				rax = g_vm->m_base_address + 0xB084; 		//lea rax, [0xFFFFFFFFF8425613]
				rcx += r11; 		//add rcx, r11
				rcx += rax; 		//add rcx, rax
				rax = r11; 		//mov rax, r11
				rax -= rbx; 		//sub rax, rbx
				rax -= 0xB9AC; 		//sub rax, 0xB9AC
				rcx ^= rax; 		//xor rcx, rax
				rax = 0x9883AEFFA28B11D0; 		//mov rax, 0x9883AEFFA28B11D0
				rcx ^= rax; 		//xor rcx, rax
				rcx *= g_vm->read<uintptr_t>(r9 + 0x15); 		//imul rcx, [r9+0x15]
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x1D; 		//shr rax, 0x1D
				rcx ^= rax; 		//xor rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x3A; 		//shr rax, 0x3A
				rcx ^= rax; 		//xor rcx, rax
				return rcx;
			}
			case 15:
			{
				r9 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599FD9); 		//mov r9, [0x00000000059B43FE]
				rbx = g_vm->m_base_address; 		//lea rbx, [0xFFFFFFFFF841A41E]
				rcx ^= rbx; 		//xor rcx, rbx
				rax = 0x62EE0D555FE38340; 		//mov rax, 0x62EE0D555FE38340
				rcx -= rax; 		//sub rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x24; 		//shr rax, 0x24
				rcx ^= rax; 		//xor rcx, rax
				rcx *= g_vm->read<uintptr_t>(r9 + 0x15); 		//imul rcx, [r9+0x15]
				rax = 0x8821DA37E07E4065; 		//mov rax, 0x8821DA37E07E4065
				rcx *= rax; 		//imul rcx, rax
				rcx += rbx; 		//add rcx, rbx
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x14; 		//shr rax, 0x14
				rcx ^= rax; 		//xor rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x28; 		//shr rax, 0x28
				rcx ^= rax; 		//xor rcx, rax
				rax = 0x6535948A0956C067; 		//mov rax, 0x6535948A0956C067
				rcx *= rax; 		//imul rcx, rax
				return rcx;
			}
			}
		}
		uintptr_t decrypt_client_active()
		{
			const uint64_t mb = g_vm->m_base_address;
			uint64_t rax = mb, rbx = mb, rcx = mb, rdx = mb, rdi = mb, rsi = mb, r8 = mb, r9 = mb, r10 = mb, r11 = mb, r12 = mb, r13 = mb, r14 = mb, r15 = mb;
			rax = g_vm->read<uintptr_t>(rdx + 0xd96a180);
			if (!rax)
				return rax;
			rbx = g_vm->m_peb; 		//mov rbx, gs:[rcx]
			// stack save: [rsp+0x58] = rdi; 		//mov [rsp+0x58], rdi
			rcx = rbx; 		//mov rcx, rbx
			// stack save: [rsp+0x50] = r12; 		//mov [rsp+0x50], r12
			rcx = _rotr64(rcx, 0x1D); 		//ror rcx, 0x1D
			// stack save: [rsp+0x48] = r13; 		//mov [rsp+0x48], r13
			rcx &= 0xF;
			switch (rcx) {
			case 0:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F7E); 		//mov r10, [0x000000000ADE42D6]
				r11 = g_vm->m_base_address; 		//lea r11, [0xFFFFFFFFFD84A351]
				rcx = 0x75A35BF5EC482D53; 		//mov rcx, 0x75A35BF5EC482D53
				rax -= rcx; 		//sub rax, rcx
				rcx = r11 + 0x39674f96; 		//lea rcx, [r11+0x39674F96]
				rcx += rbx; 		//add rcx, rbx
				rcx ^= rax; 		//xor rcx, rax
				rdx = rcx; 		//mov rdx, rcx
				rdx >>= 0x7; 		//shr rdx, 0x07
				rdx ^= rcx; 		//xor rdx, rcx
				rax = rdx; 		//mov rax, rdx
				rax >>= 0xE; 		//shr rax, 0x0E
				rax ^= rdx; 		//xor rax, rdx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x1C; 		//shr rcx, 0x1C
				rcx ^= rax; 		//xor rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x38; 		//shr rax, 0x38
				rax ^= rcx; 		//xor rax, rcx
				rcx = 0x7ED1321242287D37; 		//mov rcx, 0x7ED1321242287D37
				rax *= rcx; 		//imul rax, rcx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x2; 		//shr rcx, 0x02
				rcx ^= rax; 		//xor rcx, rax
				rdx = rcx; 		//mov rdx, rcx
				rdx >>= 0x4; 		//shr rdx, 0x04
				rdx ^= rcx; 		//xor rdx, rcx
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x8; 		//shr rax, 0x08
				rax ^= rdx; 		//xor rax, rdx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x10; 		//shr rcx, 0x10
				rax ^= rcx; 		//xor rax, rcx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x20; 		//shr rcx, 0x20
				rcx ^= rax; 		//xor rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x22; 		//shr rax, 0x22
				rax ^= rcx; 		//xor rax, rcx
				rax += r11; 		//add rax, r11
				rcx = 0; 		//and rcx, 0xFFFFFFFFC0000000
				rcx = _rotl64(rcx, 0x10); 		//rol rcx, 0x10
				rcx ^= r10; 		//xor rcx, r10
				rcx = ~rcx; 		//not rcx
				rax *= g_vm->read<uintptr_t>(rcx + 0xf); 		//imul rax, [rcx+0x0F]
				return rax;
			}
			case 1:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F7E); 		//mov r10, [0x000000000ADE3DCB]
				r11 = g_vm->m_base_address; 		//lea r11, [0xFFFFFFFFFD849E46]
				r13 = g_vm->m_base_address + 0x2B0E; 		//lea r13, [0xFFFFFFFFFD84C93D]
				r12 = g_vm->m_base_address + 0x2982AAEB; 		//lea r12, [0x000000002707490E]
				rdx = rbx; 		//mov rdx, rbx
				rdx = ~rdx; 		//not rdx
				rdx ^= r13; 		//xor rdx, r13
				rcx = r11; 		//mov rcx, r11
				rcx -= rdx; 		//sub rcx, rdx
				rax += rcx; 		//add rax, rcx
				rcx = 0; 		//and rcx, 0xFFFFFFFFC0000000
				rcx = _rotl64(rcx, 0x10); 		//rol rcx, 0x10
				rcx ^= r10; 		//xor rcx, r10
				rcx = ~rcx; 		//not rcx
				rax *= g_vm->read<uintptr_t>(rcx + 0xf); 		//imul rax, [rcx+0x0F]
				rcx = rbx; 		//mov rcx, rbx
				rcx ^= r12; 		//xor rcx, r12
				rax -= rcx; 		//sub rax, rcx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x5; 		//shr rcx, 0x05
				rax ^= rcx; 		//xor rax, rcx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0xA; 		//shr rcx, 0x0A
				rax ^= rcx; 		//xor rax, rcx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x14; 		//shr rcx, 0x14
				rcx ^= rax; 		//xor rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x28; 		//shr rax, 0x28
				rax ^= rcx; 		//xor rax, rcx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x10; 		//shr rcx, 0x10
				rax ^= rcx; 		//xor rax, rcx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x20; 		//shr rcx, 0x20
				rax ^= rcx; 		//xor rax, rcx
				rcx = 0x395F229C1F64329; 		//mov rcx, 0x395F229C1F64329
				rax *= rcx; 		//imul rax, rcx
				rcx = 0x90CE9B89EFABB41; 		//mov rcx, 0x90CE9B89EFABB41
				rax -= rcx; 		//sub rax, rcx
				return rax;
			}
			case 2:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F7E); 		//mov r10, [0x000000000ADE3853]
				r14 = g_vm->m_base_address + 0x30F0A65E; 		//lea r14, [0x000000002E753F20]
				r15 = g_vm->m_base_address + 0x8EAC; 		//lea r15, [0xFFFFFFFFFD852763]
				rdx = rbx; 		//mov rdx, rbx
				rdx = ~rdx; 		//not rdx
				rax += r15; 		//add rax, r15
				rcx = rbx; 		//mov rcx, rbx
				rcx *= r14; 		//imul rcx, r14
				rdx -= rcx; 		//sub rdx, rcx
				rax += rdx; 		//add rax, rdx
				rcx = 0xF7070315E5585D71; 		//mov rcx, 0xF7070315E5585D71
				rax ^= rcx; 		//xor rax, rcx
				rcx = 0x5C10EC95D52AE35C; 		//mov rcx, 0x5C10EC95D52AE35C
				rax += rcx; 		//add rax, rcx
				rcx = 0; 		//and rcx, 0xFFFFFFFFC0000000
				rcx = _rotl64(rcx, 0x10); 		//rol rcx, 0x10
				rcx ^= r10; 		//xor rcx, r10
				rcx = ~rcx; 		//not rcx
				rax *= g_vm->read<uintptr_t>(rcx + 0xf); 		//imul rax, [rcx+0x0F]
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x9; 		//shr rcx, 0x09
				rcx ^= rax; 		//xor rcx, rax
				rdx = rcx; 		//mov rdx, rcx
				rdx >>= 0x12; 		//shr rdx, 0x12
				rdx ^= rcx; 		//xor rdx, rcx
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x24; 		//shr rax, 0x24
				rax ^= rdx; 		//xor rax, rdx
				rcx = 0xEAF8DD4E7DAEE839; 		//mov rcx, 0xEAF8DD4E7DAEE839
				rax *= rcx; 		//imul rax, rcx
				rax -= rbx; 		//sub rax, rbx
				return rax;
			}
			case 3:
			{
				r9 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F7E); 		//mov r9, [0x000000000ADE33BF]
				r11 = g_vm->m_base_address; 		//lea r11, [0xFFFFFFFFFD84943A]
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x20; 		//shr rcx, 0x20
				rax ^= rcx; 		//xor rax, rcx
				rcx = 0x5EF0345657B7395; 		//mov rcx, 0x5EF0345657B7395
				rax *= rcx; 		//imul rax, rcx
				rcx = 0xACDBE6CC0EC7F1D6; 		//mov rcx, 0xACDBE6CC0EC7F1D6
				rax ^= rcx; 		//xor rax, rcx
				rax += rbx; 		//add rax, rbx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x28; 		//shr rcx, 0x28
				rax ^= rcx; 		//xor rax, rcx
				rax ^= r11; 		//xor rax, r11
				rcx = 0; 		//and rcx, 0xFFFFFFFFC0000000
				rcx = _rotl64(rcx, 0x10); 		//rol rcx, 0x10
				rcx ^= r9; 		//xor rcx, r9
				rcx = ~rcx; 		//not rcx
				rax *= g_vm->read<uintptr_t>(rcx + 0xf); 		//imul rax, [rcx+0x0F]
				rax += r11; 		//add rax, r11
				return rax;
			}
			case 4:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F7E); 		//mov r10, [0x000000000ADE2F53]
				r12 = g_vm->m_base_address + 0xA062; 		//lea r12, [0xFFFFFFFFFD853024]
				r15 = g_vm->m_base_address + 0x29D008DE; 		//lea r15, [0x0000000027549895]
				rcx = 0x61F81B41A154355F; 		//mov rcx, 0x61F81B41A154355F
				rax += rcx; 		//add rax, rcx
				rcx = 0; 		//and rcx, 0xFFFFFFFFC0000000
				rcx = _rotl64(rcx, 0x10); 		//rol rcx, 0x10
				rcx ^= r10; 		//xor rcx, r10
				rcx = ~rcx; 		//not rcx
				rax *= g_vm->read<uintptr_t>(rcx + 0xf); 		//imul rax, [rcx+0x0F]
				rcx = 0x60C7116DFD4132B; 		//mov rcx, 0x60C7116DFD4132B
				rax *= rcx; 		//imul rax, rcx
				rcx = rax; 		//mov rcx, rax
				rax >>= 0x3; 		//shr rax, 0x03
				rax ^= rcx; 		//xor rax, rcx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x6; 		//shr rcx, 0x06
				rcx ^= rax; 		//xor rcx, rax
				rdx = rcx; 		//mov rdx, rcx
				rdx >>= 0xC; 		//shr rdx, 0x0C
				rdx ^= rcx; 		//xor rdx, rcx
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x18; 		//shr rax, 0x18
				rax ^= rdx; 		//xor rax, rdx
				rdx = rax; 		//mov rdx, rax
				rdx >>= 0x30; 		//shr rdx, 0x30
				rdx ^= rax; 		//xor rdx, rax
				rax = rbx; 		//mov rax, rbx
				rax *= r15; 		//imul rax, r15
				rax += rdx; 		//add rax, rdx
				rdx = rbx; 		//mov rdx, rbx
				rdx *= r12; 		//imul rdx, r12
				rdx ^= rax; 		//xor rdx, rax
				rcx = rdx; 		//mov rcx, rdx
				rcx >>= 0x6; 		//shr rcx, 0x06
				rcx ^= rdx; 		//xor rcx, rdx
				rax = rcx; 		//mov rax, rcx
				rax >>= 0xC; 		//shr rax, 0x0C
				rax ^= rcx; 		//xor rax, rcx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x18; 		//shr rcx, 0x18
				rax ^= rcx; 		//xor rax, rcx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x30; 		//shr rcx, 0x30
				rax ^= rcx; 		//xor rax, rcx
				rcx = 0x7383B429035656CC; 		//mov rcx, 0x7383B429035656CC
				rax -= rcx; 		//sub rax, rcx
				return rax;
			}
			case 5:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F7E); 		//mov r10, [0x000000000ADE2A25]
				r11 = g_vm->m_base_address; 		//lea r11, [0xFFFFFFFFFD848AA0]
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x1C; 		//shr rcx, 0x1C
				rcx ^= rax; 		//xor rcx, rax
				rdx = rcx; 		//mov rdx, rcx
				rdx >>= 0x38; 		//shr rdx, 0x38
				rdx ^= rcx; 		//xor rdx, rcx
				rax = rdx; 		//mov rax, rdx
				rax >>= 0xD; 		//shr rax, 0x0D
				rax ^= rdx; 		//xor rax, rdx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x1A; 		//shr rcx, 0x1A
				rax ^= rcx; 		//xor rax, rcx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x34; 		//shr rcx, 0x34
				rax ^= rcx; 		//xor rax, rcx
				rcx = 0xFA0DDD2C604B9D1F; 		//mov rcx, 0xFA0DDD2C604B9D1F
				rax *= rcx; 		//imul rax, rcx
				rcx = g_vm->m_base_address + 0x46B0; 		//lea rcx, [0xFFFFFFFFFD84CF69]
				rax -= rcx; 		//sub rax, rcx
				rax += rbx; 		//add rax, rbx
				rcx = 0; 		//and rcx, 0xFFFFFFFFC0000000
				rcx = _rotl64(rcx, 0x10); 		//rol rcx, 0x10
				rcx ^= r10; 		//xor rcx, r10
				rcx = ~rcx; 		//not rcx
				rax *= g_vm->read<uintptr_t>(rcx + 0xf); 		//imul rax, [rcx+0x0F]
				rcx = 0x64F0F35E29D4C7B9; 		//mov rcx, 0x64F0F35E29D4C7B9
				rax *= rcx; 		//imul rax, rcx
				rax -= r11; 		//sub rax, r11
				rax += 0xFFFFFFFFFFFF24CA; 		//add rax, 0xFFFFFFFFFFFF24CA
				rax += rbx; 		//add rax, rbx
				rcx = 0x249EDB181C0475B9; 		//mov rcx, 0x249EDB181C0475B9
				rax ^= rcx; 		//xor rax, rcx
				return rax;
			}
			case 6:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F7E); 		//mov r10, [0x000000000ADE2600]
				r11 = g_vm->m_base_address; 		//lea r11, [0xFFFFFFFFFD848670]
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x10; 		//shr rcx, 0x10
				rax ^= rcx; 		//xor rax, rcx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x20; 		//shr rcx, 0x20
				rax ^= rcx; 		//xor rax, rcx
				rax += rbx; 		//add rax, rbx
				rdx = 0; 		//and rdx, 0xFFFFFFFFC0000000
				rdx = _rotl64(rdx, 0x10); 		//rol rdx, 0x10
				rdx ^= r10; 		//xor rdx, r10
				rcx = rbx; 		//mov rcx, rbx
				rcx -= r11; 		//sub rcx, r11
				r15 = 0xD29D6FF40A4A734D; 		//mov r15, 0xD29D6FF40A4A734D
				rcx += r15; 		//add rcx, r15
				rdx = ~rdx; 		//not rdx
				rax += rcx; 		//add rax, rcx
				rax *= g_vm->read<uintptr_t>(rdx + 0xf); 		//imul rax, [rdx+0x0F]
				rcx = rbx; 		//mov rcx, rbx
				rcx -= r11; 		//sub rcx, r11
				rcx -= 0x160C0564; 		//sub rcx, 0x160C0564
				rax ^= rcx; 		//xor rax, rcx
				rcx = rbx; 		//mov rcx, rbx
				rcx = ~rcx; 		//not rcx
				rcx -= r11; 		//sub rcx, r11
				rcx -= 0x473E; 		//sub rcx, 0x473E
				rax ^= rcx; 		//xor rax, rcx
				rcx = 0xA4D2A49AD11FF335; 		//mov rcx, 0xA4D2A49AD11FF335
				rax *= rcx; 		//imul rax, rcx
				return rax;
			}
			case 7:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F7E); 		//mov r10, [0x000000000ADE2163]
				r11 = g_vm->m_base_address; 		//lea r11, [0xFFFFFFFFFD8481DE]
				rcx = 0x8A9DA3385FC38B18; 		//mov rcx, 0x8A9DA3385FC38B18
				rax ^= rcx; 		//xor rax, rcx
				rax ^= rbx; 		//xor rax, rbx
				rdx = 0; 		//and rdx, 0xFFFFFFFFC0000000
				rdx = _rotl64(rdx, 0x10); 		//rol rdx, 0x10
				rdx ^= r10; 		//xor rdx, r10
				rdx = ~rdx; 		//not rdx
				rax *= g_vm->read<uintptr_t>(rdx + 0xf); 		//imul rax, [rdx+0x0F]
				rdx = rbx; 		//mov rdx, rbx
				rcx = g_vm->m_base_address + 0x49AEB26D; 		//lea rcx, [0x0000000047333371]
				rdx = ~rdx; 		//not rdx
				rcx = ~rcx; 		//not rcx
				rdx *= rcx; 		//imul rdx, rcx
				rax ^= rdx; 		//xor rax, rdx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0xF; 		//shr rcx, 0x0F
				rax ^= rcx; 		//xor rax, rcx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x1E; 		//shr rcx, 0x1E
				rax ^= rcx; 		//xor rax, rcx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x3C; 		//shr rcx, 0x3C
				rax ^= rcx; 		//xor rax, rcx
				rcx = 0xF4D742FA3C9800A5; 		//mov rcx, 0xF4D742FA3C9800A5
				rax *= rcx; 		//imul rax, rcx
				rcx = 0x10F2E1DC08054DCE; 		//mov rcx, 0x10F2E1DC08054DCE
				rax ^= r11; 		//xor rax, r11
				rax -= rcx; 		//sub rax, rcx
				return rax;
			}
			case 8:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F7E); 		//mov r10, [0x000000000ADE1D0D]
				r11 = g_vm->m_base_address; 		//lea r11, [0xFFFFFFFFFD847D88]
				rcx = 0; 		//and rcx, 0xFFFFFFFFC0000000
				rcx = _rotl64(rcx, 0x10); 		//rol rcx, 0x10
				rcx ^= r10; 		//xor rcx, r10
				rcx = ~rcx; 		//not rcx
				rax *= g_vm->read<uintptr_t>(rcx + 0xf); 		//imul rax, [rcx+0x0F]
				rcx = rax; 		//mov rcx, rax
				rax >>= 0x7; 		//shr rax, 0x07
				rax ^= rcx; 		//xor rax, rcx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0xE; 		//shr rcx, 0x0E
				rcx ^= rax; 		//xor rcx, rax
				rdx = rcx; 		//mov rdx, rcx
				rdx >>= 0x1C; 		//shr rdx, 0x1C
				rdx ^= rcx; 		//xor rdx, rcx
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x38; 		//shr rax, 0x38
				rax ^= rdx; 		//xor rax, rdx
				rcx = 0x95279C81D5942D9; 		//mov rcx, 0x95279C81D5942D9
				rax *= rcx; 		//imul rax, rcx
				rcx = 0x43F7B4F4DCCCEAA1; 		//mov rcx, 0x43F7B4F4DCCCEAA1
				rax -= r11; 		//sub rax, r11
				rax -= rcx; 		//sub rax, rcx
				rax ^= rbx; 		//xor rax, rbx
				rax ^= r11; 		//xor rax, r11
				rcx = 0x41A42223B53D6ABF; 		//mov rcx, 0x41A42223B53D6ABF
				rax *= rcx; 		//imul rax, rcx
				return rax;
			}
			case 9:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F7E); 		//mov r10, [0x000000000ADE18F9]
				r11 = g_vm->m_base_address; 		//lea r11, [0xFFFFFFFFFD847974]
				rax ^= r11; 		//xor rax, r11
				rcx = 0; 		//and rcx, 0xFFFFFFFFC0000000
				rcx = _rotl64(rcx, 0x10); 		//rol rcx, 0x10
				rcx ^= r10; 		//xor rcx, r10
				rcx = ~rcx; 		//not rcx
				rax *= g_vm->read<uintptr_t>(rcx + 0xf); 		//imul rax, [rcx+0x0F]
				rax += rbx; 		//add rax, rbx
				rax ^= rbx; 		//xor rax, rbx
				rax -= r11; 		//sub rax, r11
				rcx = 0x4B6230D25E9BA161; 		//mov rcx, 0x4B6230D25E9BA161
				rcx *= rax; 		//imul rcx, rax
				rcx += rbx; 		//add rcx, rbx
				rdx = rcx; 		//mov rdx, rcx
				rdx >>= 0xE; 		//shr rdx, 0x0E
				rdx ^= rcx; 		//xor rdx, rcx
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x1C; 		//shr rax, 0x1C
				rax ^= rdx; 		//xor rax, rdx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x38; 		//shr rcx, 0x38
				rax ^= rcx; 		//xor rax, rcx
				return rax;
			}
			case 10:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F7E); 		//mov r10, [0x000000000ADE1454]
				r11 = g_vm->m_base_address; 		//lea r11, [0xFFFFFFFFFD8474C4]
				r15 = g_vm->m_base_address + 0x2BB81C0C; 		//lea r15, [0x00000000293C90B9]
				rcx = 0; 		//and rcx, 0xFFFFFFFFC0000000
				rcx = _rotl64(rcx, 0x10); 		//rol rcx, 0x10
				rcx ^= r10; 		//xor rcx, r10
				rcx = ~rcx; 		//not rcx
				rax *= g_vm->read<uintptr_t>(rcx + 0xf); 		//imul rax, [rcx+0x0F]
				rcx = rbx; 		//mov rcx, rbx
				rcx -= r11; 		//sub rcx, r11
				rcx += 0xFFFFFFFFFFFFCAD7; 		//add rcx, 0xFFFFFFFFFFFFCAD7
				rax += rcx; 		//add rax, rcx
				rcx = rbx; 		//mov rcx, rbx
				rcx = ~rcx; 		//not rcx
				rcx += r15; 		//add rcx, r15
				rcx ^= rax; 		//xor rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x12; 		//shr rax, 0x12
				rax ^= rcx; 		//xor rax, rcx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x24; 		//shr rcx, 0x24
				rax ^= rcx; 		//xor rax, rcx
				rdx = rbx; 		//mov rdx, rbx
				rcx = g_vm->m_base_address + 0xEF52; 		//lea rcx, [0xFFFFFFFFFD856292]
				rdx = ~rdx; 		//not rdx
				rax += rcx; 		//add rax, rcx
				rax += rdx; 		//add rax, rdx
				rcx = 0x6763BC8B6D17752D; 		//mov rcx, 0x6763BC8B6D17752D
				rax *= rcx; 		//imul rax, rcx
				rcx = 0x8BEF7389669EF866; 		//mov rcx, 0x8BEF7389669EF866
				rax ^= rcx; 		//xor rax, rcx
				return rax;
			}
			case 11:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F7E); 		//mov r10, [0x000000000ADE0F38]
				rcx = 0; 		//and rcx, 0xFFFFFFFFC0000000
				rcx = _rotl64(rcx, 0x10); 		//rol rcx, 0x10
				rcx ^= r10; 		//xor rcx, r10
				rcx = ~rcx; 		//not rcx
				rax *= g_vm->read<uintptr_t>(rcx + 0xf); 		//imul rax, [rcx+0x0F]
				rax ^= rbx; 		//xor rax, rbx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x1F; 		//shr rcx, 0x1F
				rcx ^= rax; 		//xor rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x3E; 		//shr rax, 0x3E
				rax ^= rcx; 		//xor rax, rcx
				rdx = rbx; 		//mov rdx, rbx
				rdx = ~rdx; 		//not rdx
				rcx = g_vm->m_base_address + 0x923A; 		//lea rcx, [0xFFFFFFFFFD84FFCE]
				rcx = ~rcx; 		//not rcx
				rdx += rcx; 		//add rdx, rcx
				rax ^= rdx; 		//xor rax, rdx
				rax += rbx; 		//add rax, rbx
				rcx = 0x9FC5A5033F4BA222; 		//mov rcx, 0x9FC5A5033F4BA222
				rax ^= rcx; 		//xor rax, rcx
				rcx = 0xEE85007B158F0715; 		//mov rcx, 0xEE85007B158F0715
				rax *= rcx; 		//imul rax, rcx
				rcx = 0x237F4FB43D19B0AB; 		//mov rcx, 0x237F4FB43D19B0AB
				rax ^= rcx; 		//xor rax, rcx
				return rax;
			}
			case 12:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F7E); 		//mov r10, [0x000000000ADE0A68]
				r11 = g_vm->m_base_address + 0x12A67ACE; 		//lea r11, [0x00000000102AE5A5]
				rcx = 0x1A8D56D056CAA6AD; 		//mov rcx, 0x1A8D56D056CAA6AD
				rax *= rcx; 		//imul rax, rcx
				rcx = r11; 		//mov rcx, r11
				rcx = ~rcx; 		//not rcx
				rcx -= rbx; 		//sub rcx, rbx
				rax ^= rcx; 		//xor rax, rcx
				rax ^= rbx; 		//xor rax, rbx
				rcx = 0xCF3780F20368ADFF; 		//mov rcx, 0xCF3780F20368ADFF
				rax *= rcx; 		//imul rax, rcx
				rdx = rax; 		//mov rdx, rax
				rdx >>= 0x15; 		//shr rdx, 0x15
				rdx ^= rax; 		//xor rdx, rax
				rcx = 0; 		//and rcx, 0xFFFFFFFFC0000000
				rcx = _rotl64(rcx, 0x10); 		//rol rcx, 0x10
				rax = rdx; 		//mov rax, rdx
				rcx ^= r10; 		//xor rcx, r10
				rax >>= 0x2A; 		//shr rax, 0x2A
				rax ^= rdx; 		//xor rax, rdx
				rcx = ~rcx; 		//not rcx
				rax *= g_vm->read<uintptr_t>(rcx + 0xf); 		//imul rax, [rcx+0x0F]
				rcx = 0x620FE8A3984FA9A2; 		//mov rcx, 0x620FE8A3984FA9A2
				rax ^= rcx; 		//xor rax, rcx
				rax += rbx; 		//add rax, rbx
				return rax;
			}
			case 13:
			{
				r9 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F7E); 		//mov r9, [0x000000000ADE06DC]
				r11 = g_vm->m_base_address; 		//lea r11, [0xFFFFFFFFFD846757]
				rax += r11; 		//add rax, r11
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x10; 		//shr rcx, 0x10
				rcx ^= rax; 		//xor rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x20; 		//shr rax, 0x20
				rax ^= rcx; 		//xor rax, rcx
				rcx = 0x153249CC4096DEBB; 		//mov rcx, 0x153249CC4096DEBB
				rax += rcx; 		//add rax, rcx
				rax += rbx; 		//add rax, rbx
				rax ^= r11; 		//xor rax, r11
				rcx = 0x3FEF9376461CD77C; 		//mov rcx, 0x3FEF9376461CD77C
				rax += rcx; 		//add rax, rcx
				rcx = 0; 		//and rcx, 0xFFFFFFFFC0000000
				rcx = _rotl64(rcx, 0x10); 		//rol rcx, 0x10
				rcx ^= r9; 		//xor rcx, r9
				rcx = ~rcx; 		//not rcx
				rax *= g_vm->read<uintptr_t>(rcx + 0xf); 		//imul rax, [rcx+0x0F]
				rcx = 0xCD5C890FC6052E0B; 		//mov rcx, 0xCD5C890FC6052E0B
				rax *= rcx; 		//imul rax, rcx
				return rax;
			}
			case 14:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F7E); 		//mov r10, [0x000000000ADE01E0]
				r11 = g_vm->m_base_address; 		//lea r11, [0xFFFFFFFFFD84625B]
				r12 = g_vm->m_base_address + 0x2858; 		//lea r12, [0xFFFFFFFFFD848A91]
				r13 = g_vm->m_base_address + 0xB818; 		//lea r13, [0xFFFFFFFFFD851A44]
				rcx = 0; 		//and rcx, 0xFFFFFFFFC0000000
				rcx = _rotl64(rcx, 0x10); 		//rol rcx, 0x10
				rcx ^= r10; 		//xor rcx, r10
				rcx = ~rcx; 		//not rcx
				rax *= g_vm->read<uintptr_t>(rcx + 0xf); 		//imul rax, [rcx+0x0F]
				rax -= r11; 		//sub rax, r11
				rcx = rax; 		//mov rcx, rax
				rdx = rbx; 		//mov rdx, rbx
				rdx *= r13; 		//imul rdx, r13
				rcx >>= 0x28; 		//shr rcx, 0x28
				rdx ^= rcx; 		//xor rdx, rcx
				rax ^= rdx; 		//xor rax, rdx
				rcx = 0x51E63BDB2878DA3D; 		//mov rcx, 0x51E63BDB2878DA3D
				rax *= rcx; 		//imul rax, rcx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x17; 		//shr rcx, 0x17
				rcx ^= rax; 		//xor rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x2E; 		//shr rax, 0x2E
				rax ^= rcx; 		//xor rax, rcx
				rcx = g_vm->m_base_address + 0x54FCAA3A; 		//lea rcx, [0x0000000052810966]
				rcx = ~rcx; 		//not rcx
				rcx ^= rbx; 		//xor rcx, rbx
				rax -= rcx; 		//sub rax, rcx
				rcx = rbx; 		//mov rcx, rbx
				rcx ^= r12; 		//xor rcx, r12
				rax -= rcx; 		//sub rax, rcx
				return rax;
			}
			case 15:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD599F7E); 		//mov r10, [0x000000000ADDFDB5]
				r11 = g_vm->m_base_address; 		//lea r11, [0xFFFFFFFFFD845E30]
				r12 = g_vm->m_base_address + 0x6D7996B9; 		//lea r12, [0x000000006AFDF4C7]
				rax += r11; 		//add rax, r11
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x16; 		//shr rcx, 0x16
				rax ^= rcx; 		//xor rax, rcx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x2C; 		//shr rcx, 0x2C
				rax ^= rcx; 		//xor rax, rcx
				rcx = r12; 		//mov rcx, r12
				rcx = ~rcx; 		//not rcx
				rcx *= rbx; 		//imul rcx, rbx
				rax += rcx; 		//add rax, rcx
				rdx = 0xEEC920FB0911427A; 		//mov rdx, 0xEEC920FB0911427A
				rax += rdx; 		//add rax, rdx
				rcx = 0; 		//and rcx, 0xFFFFFFFFC0000000
				rcx = _rotl64(rcx, 0x10); 		//rol rcx, 0x10
				rcx ^= r10; 		//xor rcx, r10
				rcx = ~rcx; 		//not rcx
				rax *= g_vm->read<uintptr_t>(rcx + 0xf); 		//imul rax, [rcx+0x0F]
				rcx = g_vm->m_base_address + 0x9A06; 		//lea rcx, [0xFFFFFFFFFD84F44D]
				rcx = ~rcx; 		//not rcx
				rax += rcx; 		//add rax, rcx
				rcx = 0xC2732136FBDEB5CD; 		//mov rcx, 0xC2732136FBDEB5CD
				rax -= rbx; 		//sub rax, rbx
				rax ^= rcx; 		//xor rax, rcx
				rcx = 0x8A516BBDDE1E408F; 		//mov rcx, 0x8A516BBDDE1E408F
				rax *= rcx; 		//imul rax, rcx
				return rax;
			}
			}
		}
		uintptr_t decrypt_bone_base()
		{
			const uint64_t mb = g_vm->m_base_address;
			uint64_t rax = mb, rbx = mb, rcx = mb, rdx = mb, rdi = mb, rsi = mb, r8 = mb, r9 = mb, r10 = mb, r11 = mb, r12 = mb, r13 = mb, r14 = mb, r15 = mb;
			rdx = g_vm->read<uintptr_t>(g_vm->m_base_address + 0x132257D8);
			if (!rdx)
				return rdx;
			r11 = g_vm->m_peb; 		//mov r11, gs:[rax]
			rax = r11; 		//mov rax, r11
			rax >>= 0x13; 		//shr rax, 0x13
			rax &= 0xF;
			switch (rax) {
			case 0:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD59A04D); 		//mov r10, [0x00000000086DE26D]
				rax = g_vm->m_base_address + 0x8C93; 		//lea rax, [0xFFFFFFFFFB14CE02]
				rax -= r11; 		//sub rax, r11
				rdx ^= rax; 		//xor rdx, rax
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x15; 		//shr rax, 0x15
				rax ^= rdx; 		//xor rax, rdx
				rdx = rax; 		//mov rdx, rax
				rdx >>= 0x2A; 		//shr rdx, 0x2A
				rdx ^= rax; 		//xor rdx, rax
				rdx += r11; 		//add rdx, r11
				rax = 0; 		//and rax, 0xFFFFFFFFC0000000
				rax = _rotl64(rax, 0x10); 		//rol rax, 0x10
				rax ^= r10; 		//xor rax, r10
				rax = ~rax; 		//not rax
				rdx *= g_vm->read<uintptr_t>(rax + 0x17); 		//imul rdx, [rax+0x17]
				rax = g_vm->m_base_address; 		//lea rax, [0xFFFFFFFFFB143E80]
				rdx ^= rax; 		//xor rdx, rax
				rax = 0x860534C8C01FEA7B; 		//mov rax, 0x860534C8C01FEA7B
				rdx *= rax; 		//imul rdx, rax
				rax = 0xEE334BF3EC572D68; 		//mov rax, 0xEE334BF3EC572D68
				rdx ^= rax; 		//xor rdx, rax
				return rdx;
			}
			case 1:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD59A04D); 		//mov r10, [0x00000000086DDE14]
				r15 = g_vm->m_base_address + 0xDF5D; 		//lea r15, [0xFFFFFFFFFB151D11]
				rax = 0; 		//and rax, 0xFFFFFFFFC0000000
				rax = _rotl64(rax, 0x10); 		//rol rax, 0x10
				rax ^= r10; 		//xor rax, r10
				rax = ~rax; 		//not rax
				rdx *= g_vm->read<uintptr_t>(rax + 0x17); 		//imul rdx, [rax+0x17]
				rax = 0x8A4B98169395E686; 		//mov rax, 0x8A4B98169395E686
				rdx ^= rax; 		//xor rdx, rax
				rax = 0xC3957EB9F84EC5AF; 		//mov rax, 0xC3957EB9F84EC5AF
				rdx *= rax; 		//imul rdx, rax
				rax = rdx; 		//mov rax, rdx
				rax >>= 0xE; 		//shr rax, 0x0E
				rax ^= rdx; 		//xor rax, rdx
				rdx = rax; 		//mov rdx, rax
				rdx >>= 0x1C; 		//shr rdx, 0x1C
				rdx ^= rax; 		//xor rdx, rax
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x38; 		//shr rax, 0x38
				rdx ^= rax; 		//xor rdx, rax
				rax = g_vm->m_base_address + 0x31CB; 		//lea rax, [0xFFFFFFFFFB146B56]
				rax -= r11; 		//sub rax, r11
				rdx += rax; 		//add rdx, rax
				rax = rdx; 		//mov rax, rdx
				rax >>= 0xD; 		//shr rax, 0x0D
				rax ^= rdx; 		//xor rax, rdx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x1A; 		//shr rcx, 0x1A
				rcx ^= rax; 		//xor rcx, rax
				rdx = rcx; 		//mov rdx, rcx
				rdx >>= 0x34; 		//shr rdx, 0x34
				rdx ^= rcx; 		//xor rdx, rcx
				rax = r15; 		//mov rax, r15
				rax = ~rax; 		//not rax
				rdx ^= rax; 		//xor rdx, rax
				rdx ^= r11; 		//xor rdx, r11
				return rdx;
			}
			case 2:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD59A04D); 		//mov r10, [0x00000000086DD90F]
				r12 = g_vm->m_base_address + 0x47C2AE1B; 		//lea r12, [0x0000000042D6E6CA]
				rax = 0; 		//and rax, 0xFFFFFFFFC0000000
				rax = _rotl64(rax, 0x10); 		//rol rax, 0x10
				rax ^= r10; 		//xor rax, r10
				rax = ~rax; 		//not rax
				rdx *= g_vm->read<uintptr_t>(rax + 0x17); 		//imul rdx, [rax+0x17]
				rax = r12; 		//mov rax, r12
				rax = ~rax; 		//not rax
				rax ^= r11; 		//xor rax, r11
				rdx += rax; 		//add rdx, rax
				rax = 0x94073D91C803188D; 		//mov rax, 0x94073D91C803188D
				rdx += r11; 		//add rdx, r11
				rdx ^= rax; 		//xor rdx, rax
				rax = 0x2EEA8A0831CE333B; 		//mov rax, 0x2EEA8A0831CE333B
				rdx *= rax; 		//imul rdx, rax
				rdx += r11; 		//add rdx, r11
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x13; 		//shr rax, 0x13
				rdx ^= rax; 		//xor rdx, rax
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x26; 		//shr rax, 0x26
				rdx ^= rax; 		//xor rdx, rax
				rax = 0xD4E2CCE5B7959CA0; 		//mov rax, 0xD4E2CCE5B7959CA0
				rdx ^= rax; 		//xor rdx, rax
				return rdx;
			}
			case 3:
			{
				r9 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD59A04D); 		//mov r9, [0x00000000086DD550]
				r12 = g_vm->m_base_address + 0x114B; 		//lea r12, [0xFFFFFFFFFB14463B]
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x13; 		//shr rax, 0x13
				rax ^= rdx; 		//xor rax, rdx
				rdx = rax; 		//mov rdx, rax
				rdx >>= 0x26; 		//shr rdx, 0x26
				rdx ^= rax; 		//xor rdx, rax
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x24; 		//shr rax, 0x24
				rdx ^= rax; 		//xor rdx, rax
				rax = 0x764F15DD269101D3; 		//mov rax, 0x764F15DD269101D3
				rdx *= rax; 		//imul rdx, rax
				rax = 0x34E81942B113C230; 		//mov rax, 0x34E81942B113C230
				rdx -= rax; 		//sub rdx, rax
				rax = 0x13805FC46F4FC36A; 		//mov rax, 0x13805FC46F4FC36A
				rdx += rax; 		//add rdx, rax
				rax = r11; 		//mov rax, r11
				rax -= g_vm->m_base_address; 		//sub rax, [rsp+0x88] -- didn't find trace -> use base[0m
				rax += 0xFFFFFFFFFFFF85F3; 		//add rax, 0xFFFFFFFFFFFF85F3
				rdx += rax; 		//add rdx, rax
				rax = 0; 		//and rax, 0xFFFFFFFFC0000000
				rax = _rotl64(rax, 0x10); 		//rol rax, 0x10
				rax ^= r9; 		//xor rax, r9
				rax = ~rax; 		//not rax
				rdx *= g_vm->read<uintptr_t>(rax + 0x17); 		//imul rdx, [rax+0x17]
				rdx ^= r12; 		//xor rdx, r12
				rdx ^= r11; 		//xor rdx, r11
				return rdx;
			}
			case 4:
			{
				r9 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD59A04D); 		//mov r9, [0x00000000086DCFC6]
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x11; 		//shr rax, 0x11
				rdx ^= rax; 		//xor rdx, rax
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x22; 		//shr rax, 0x22
				rdx ^= rax; 		//xor rdx, rax
				rax = 0x2CFB6FB2F3BAD3C; 		//mov rax, 0x2CFB6FB2F3BAD3C
				rdx -= rax; 		//sub rdx, rax
				rax = 0; 		//and rax, 0xFFFFFFFFC0000000
				rax = _rotl64(rax, 0x10); 		//rol rax, 0x10
				rax ^= r9; 		//xor rax, r9
				rax = ~rax; 		//not rax
				rdx *= g_vm->read<uintptr_t>(rax + 0x17); 		//imul rdx, [rax+0x17]
				rax = 0xEED0F28134CE8447; 		//mov rax, 0xEED0F28134CE8447
				rdx *= rax; 		//imul rdx, rax
				rax = 0x52D4170A67BFFCB2; 		//mov rax, 0x52D4170A67BFFCB2
				rdx ^= rax; 		//xor rdx, rax
				rax = rdx + r11 * 1; 		//lea rax, [rdx+r11*1]
				rdx = rax; 		//mov rdx, rax
				rdx >>= 0x16; 		//shr rdx, 0x16
				rdx ^= rax; 		//xor rdx, rax
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x2C; 		//shr rax, 0x2C
				rdx ^= rax; 		//xor rdx, rax
				rdx ^= r11; 		//xor rdx, r11
				return rdx;
			}
			case 5:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD59A04D); 		//mov r10, [0x00000000086DCA0F]
				r15 = g_vm->m_base_address + 0x19B7DBCB; 		//lea r15, [0x0000000014CC0575]
				r12 = g_vm->m_base_address + 0x654BDD13; 		//lea r12, [0x00000000606006AC]
				rax = r12; 		//mov rax, r12
				rax = ~rax; 		//not rax
				rax++; 		//inc rax
				rdx += rax; 		//add rdx, rax
				rax = 0x4A2AFA53025C5181; 		//mov rax, 0x4A2AFA53025C5181
				rdx += r11; 		//add rdx, r11
				rdx *= rax; 		//imul rdx, rax
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x28; 		//shr rax, 0x28
				rdx ^= rax; 		//xor rdx, rax
				rax = r11 + r15 * 1; 		//lea rax, [r11+r15*1]
				rcx = g_vm->m_base_address + 0xA045; 		//lea rcx, [0xFFFFFFFFFB14C6C5]
				rcx += r11; 		//add rcx, r11
				rcx ^= rax; 		//xor rcx, rax
				rdx ^= rcx; 		//xor rdx, rcx
				rax = 0x574A3A5B7408079B; 		//mov rax, 0x574A3A5B7408079B
				rdx *= rax; 		//imul rdx, rax
				rax = 0; 		//and rax, 0xFFFFFFFFC0000000
				rax = _rotl64(rax, 0x10); 		//rol rax, 0x10
				rax ^= r10; 		//xor rax, r10
				rax = ~rax; 		//not rax
				rdx *= g_vm->read<uintptr_t>(rax + 0x17); 		//imul rdx, [rax+0x17]
				return rdx;
			}
			case 6:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD59A04D); 		//mov r10, [0x00000000086DC624]
				r15 = g_vm->m_base_address + 0x4951; 		//lea r15, [0xFFFFFFFFFB146F15]
				rax = 0; 		//and rax, 0xFFFFFFFFC0000000
				rax = _rotl64(rax, 0x10); 		//rol rax, 0x10
				rax ^= r10; 		//xor rax, r10
				rax = ~rax; 		//not rax
				rdx *= g_vm->read<uintptr_t>(rax + 0x17); 		//imul rdx, [rax+0x17]
				rdx += r11; 		//add rdx, r11
				rax = r15; 		//mov rax, r15
				rax *= r11; 		//imul rax, r11
				rdx += rax; 		//add rdx, rax
				rcx = g_vm->m_base_address; 		//lea rcx, [0xFFFFFFFFFB142402]
				rcx += 0x1D37B933; 		//add rcx, 0x1D37B933
				rcx += rdx; 		//add rcx, rdx
				rcx += r11; 		//add rcx, r11
				rdx = rcx; 		//mov rdx, rcx
				rdx >>= 0x9; 		//shr rdx, 0x09
				rdx ^= rcx; 		//xor rdx, rcx
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x12; 		//shr rax, 0x12
				rax ^= rdx; 		//xor rax, rdx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x24; 		//shr rcx, 0x24
				rcx ^= rax; 		//xor rcx, rax
				rax = 0x6C2A29044A40E4C7; 		//mov rax, 0x6C2A29044A40E4C7
				rcx *= rax; 		//imul rcx, rax
				rax = g_vm->m_base_address; 		//lea rax, [0xFFFFFFFFFB1423A8]
				rcx ^= rax; 		//xor rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0x3; 		//shr rax, 0x03
				rax ^= rcx; 		//xor rax, rcx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0x6; 		//shr rcx, 0x06
				rcx ^= rax; 		//xor rcx, rax
				rax = rcx; 		//mov rax, rcx
				rax >>= 0xC; 		//shr rax, 0x0C
				rax ^= rcx; 		//xor rax, rcx
				rdx = rax; 		//mov rdx, rax
				rdx >>= 0x18; 		//shr rdx, 0x18
				rdx ^= rax; 		//xor rdx, rax
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x30; 		//shr rax, 0x30
				rdx ^= rax; 		//xor rdx, rax
				return rdx;
			}
			case 7:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD59A04D); 		//mov r10, [0x00000000086DC217]
				r15 = g_vm->m_base_address + 0xCEFB; 		//lea r15, [0xFFFFFFFFFB14F0AD]
				rax = g_vm->m_base_address; 		//lea rax, [0xFFFFFFFFFB142080]
				rdx += rax; 		//add rdx, rax
				rax = 0x5F80490A38DB3901; 		//mov rax, 0x5F80490A38DB3901
				rdx ^= rax; 		//xor rdx, rax
				rax = 0x4EC9DC6A5902297D; 		//mov rax, 0x4EC9DC6A5902297D
				rdx -= rax; 		//sub rdx, rax
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x25; 		//shr rax, 0x25
				rdx ^= rax; 		//xor rdx, rax
				rax = r15; 		//mov rax, r15
				rax ^= r11; 		//xor rax, r11
				rdx += rax; 		//add rdx, rax
				rax = 0x92B34BC27C367071; 		//mov rax, 0x92B34BC27C367071
				rdx *= rax; 		//imul rdx, rax
				rdx -= r11; 		//sub rdx, r11
				rax = 0; 		//and rax, 0xFFFFFFFFC0000000
				rax = _rotl64(rax, 0x10); 		//rol rax, 0x10
				rax ^= r10; 		//xor rax, r10
				rax = ~rax; 		//not rax
				rdx *= g_vm->read<uintptr_t>(rax + 0x17); 		//imul rdx, [rax+0x17]
				return rdx;
			}
			case 8:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD59A04D); 		//mov r10, [0x00000000086DBDCE]
				r13 = g_vm->m_base_address + 0x5723; 		//lea r13, [0xFFFFFFFFFB147491]
				r12 = g_vm->m_base_address + 0xFAB2; 		//lea r12, [0xFFFFFFFFFB151811]
				rax = 0xE62DA6375F493113; 		//mov rax, 0xE62DA6375F493113
				rdx *= rax; 		//imul rdx, rax
				rax = g_vm->m_base_address; 		//lea rax, [0xFFFFFFFFFB141AE6]
				rdx -= rax; 		//sub rdx, rax
				rax = 0; 		//and rax, 0xFFFFFFFFC0000000
				rax = _rotl64(rax, 0x10); 		//rol rax, 0x10
				rax ^= r10; 		//xor rax, r10
				rax = ~rax; 		//not rax
				rdx *= g_vm->read<uintptr_t>(rax + 0x17); 		//imul rdx, [rax+0x17]
				rdx -= r11; 		//sub rdx, r11
				rax = rdx; 		//mov rax, rdx
				rax >>= 0xF; 		//shr rax, 0x0F
				rdx ^= rax; 		//xor rdx, rax
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x1E; 		//shr rax, 0x1E
				rdx ^= rax; 		//xor rdx, rax
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x3C; 		//shr rax, 0x3C
				rdx ^= rax; 		//xor rdx, rax
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x3; 		//shr rax, 0x03
				rax ^= rdx; 		//xor rax, rdx
				rdx = rax; 		//mov rdx, rax
				rdx >>= 0x6; 		//shr rdx, 0x06
				rdx ^= rax; 		//xor rdx, rax
				rax = rdx; 		//mov rax, rdx
				rax >>= 0xC; 		//shr rax, 0x0C
				rdx ^= rax; 		//xor rdx, rax
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x18; 		//shr rax, 0x18
				rax ^= rdx; 		//xor rax, rdx
				rdx = rax; 		//mov rdx, rax
				rdx >>= 0x30; 		//shr rdx, 0x30
				rdx ^= rax; 		//xor rdx, rax
				rax = r11 + r12 * 1; 		//lea rax, [r11+r12*1]
				rdx ^= rax; 		//xor rdx, rax
				rdx ^= r13; 		//xor rdx, r13
				rdx ^= r11; 		//xor rdx, r11
				return rdx;
			}
			case 9:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD59A04D); 		//mov r10, [0x00000000086DB897]
				r12 = g_vm->m_base_address + 0xF1EC; 		//lea r12, [0xFFFFFFFFFB150A1E]
				r13 = g_vm->m_base_address + 0x5304B0E6; 		//lea r13, [0x000000004E18C907]
				rax = 0; 		//and rax, 0xFFFFFFFFC0000000
				rax = _rotl64(rax, 0x10); 		//rol rax, 0x10
				rax ^= r10; 		//xor rax, r10
				rax = ~rax; 		//not rax
				rdx *= g_vm->read<uintptr_t>(rax + 0x17); 		//imul rdx, [rax+0x17]
				rax = r11 + r12 * 1; 		//lea rax, [r11+r12*1]
				rcx = g_vm->m_base_address; 		//lea rcx, [0xFFFFFFFFFB14178E]
				rcx += 0x429D; 		//add rcx, 0x429D
				rcx += rdx; 		//add rcx, rdx
				rcx += r11; 		//add rcx, r11
				rcx ^= rax; 		//xor rcx, rax
				rcx ^= r13; 		//xor rcx, r13
				rcx ^= r11; 		//xor rcx, r11
				rdx = rcx; 		//mov rdx, rcx
				rdx >>= 0x22; 		//shr rdx, 0x22
				rdx ^= rcx; 		//xor rdx, rcx
				rax = 0xEE899EDDAF56550; 		//mov rax, 0xEE899EDDAF56550
				rdx ^= rax; 		//xor rdx, rax
				rax = rdx; 		//mov rax, rdx
				rax >>= 0xE; 		//shr rax, 0x0E
				rdx ^= rax; 		//xor rdx, rax
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x1C; 		//shr rax, 0x1C
				rdx ^= rax; 		//xor rdx, rax
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x38; 		//shr rax, 0x38
				rdx ^= rax; 		//xor rdx, rax
				rax = 0x39D515C223A57391; 		//mov rax, 0x39D515C223A57391
				rdx *= rax; 		//imul rdx, rax
				return rdx;
			}
			case 10:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD59A04D); 		//mov r10, [0x00000000086DB42A]
				r12 = g_vm->m_base_address + 0x1A3D; 		//lea r12, [0xFFFFFFFFFB142E07]
				rax = rdx; 		//mov rax, rdx
				rax >>= 0xF; 		//shr rax, 0x0F
				rax ^= rdx; 		//xor rax, rdx
				rdx = rax; 		//mov rdx, rax
				rdx >>= 0x1E; 		//shr rdx, 0x1E
				rdx ^= rax; 		//xor rdx, rax
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x3C; 		//shr rax, 0x3C
				rax ^= rdx; 		//xor rax, rdx
				rdx = rax; 		//mov rdx, rax
				rdx >>= 0x13; 		//shr rdx, 0x13
				rdx ^= rax; 		//xor rdx, rax
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x26; 		//shr rax, 0x26
				rdx ^= rax; 		//xor rdx, rax
				rax = r11; 		//mov rax, r11
				rax = ~rax; 		//not rax
				rax *= r12; 		//imul rax, r12
				rdx ^= rax; 		//xor rdx, rax
				rax = 0x8330B389343DA675; 		//mov rax, 0x8330B389343DA675
				rdx *= rax; 		//imul rdx, rax
				rax = 0x5A325A7184C15E55; 		//mov rax, 0x5A325A7184C15E55
				rdx -= rax; 		//sub rdx, rax
				rax = 0xE28957C95B7E497; 		//mov rax, 0xE28957C95B7E497
				rdx += rax; 		//add rdx, rax
				rax = 0; 		//and rax, 0xFFFFFFFFC0000000
				rax = _rotl64(rax, 0x10); 		//rol rax, 0x10
				rax ^= r10; 		//xor rax, r10
				rax = ~rax; 		//not rax
				rdx *= g_vm->read<uintptr_t>(rax + 0x17); 		//imul rdx, [rax+0x17]
				rdx -= r11; 		//sub rdx, r11
				return rdx;
			}
			case 11:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD59A04D); 		//mov r10, [0x00000000086DAF65]
				r14 = g_vm->m_base_address + 0x67B591A2; 		//lea r14, [0x0000000062C9A0A2]
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x22; 		//shr rax, 0x22
				rdx ^= rax; 		//xor rdx, rax
				rax = r14; 		//mov rax, r14
				rax = ~rax; 		//not rax
				rax ^= r11; 		//xor rax, r11
				rax += r11; 		//add rax, r11
				rdx -= rax; 		//sub rdx, rax
				rax = 0; 		//and rax, 0xFFFFFFFFC0000000
				rax = _rotl64(rax, 0x10); 		//rol rax, 0x10
				rax ^= r10; 		//xor rax, r10
				rax = ~rax; 		//not rax
				rdx *= g_vm->read<uintptr_t>(rax + 0x17); 		//imul rdx, [rax+0x17]
				rdx ^= r11; 		//xor rdx, r11
				rax = 0x112AEF7CBA9BEDF1; 		//mov rax, 0x112AEF7CBA9BEDF1
				rdx *= rax; 		//imul rdx, rax
				rax = 0x792205E77EAA6797; 		//mov rax, 0x792205E77EAA6797
				rdx ^= rax; 		//xor rdx, rax
				return rdx;
			}
			case 12:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD59A04D); 		//mov r10, [0x00000000086DABC7]
				r15 = g_vm->m_base_address + 0x70E4B3E1; 		//lea r15, [0x000000006BF8BF43]
				rax = 0; 		//and rax, 0xFFFFFFFFC0000000
				rax = _rotl64(rax, 0x10); 		//rol rax, 0x10
				rax ^= r10; 		//xor rax, r10
				rax = ~rax; 		//not rax
				rdx *= g_vm->read<uintptr_t>(rax + 0x17); 		//imul rdx, [rax+0x17]
				rax = 0x33BF00DD8A073650; 		//mov rax, 0x33BF00DD8A073650
				rdx -= rax; 		//sub rdx, rax
				rax = rdx; 		//mov rax, rdx
				rax >>= 0xA; 		//shr rax, 0x0A
				rdx ^= rax; 		//xor rdx, rax
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x14; 		//shr rax, 0x14
				rax ^= rdx; 		//xor rax, rdx
				rdx = rax; 		//mov rdx, rax
				rdx >>= 0x28; 		//shr rdx, 0x28
				rdx ^= rax; 		//xor rdx, rax
				rax = g_vm->m_base_address; 		//lea rax, [0xFFFFFFFFFB14090E]
				rdx ^= rax; 		//xor rdx, rax
				rax = r15; 		//mov rax, r15
				rax = ~rax; 		//not rax
				rdx += rax; 		//add rdx, rax
				rax = 0x37300D9E69A77B2F; 		//mov rax, 0x37300D9E69A77B2F
				rdx *= rax; 		//imul rdx, rax
				rdx -= r11; 		//sub rdx, r11
				return rdx;
			}
			case 13:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD59A04D); 		//mov r10, [0x00000000086DA7FC]
				r14 = g_vm->m_base_address + 0x666C9DA0; 		//lea r14, [0x000000006180A537]
				rax = r14; 		//mov rax, r14
				rax ^= r11; 		//xor rax, r11
				rdx -= rax; 		//sub rdx, rax
				rax = 0x124569EA4125D98; 		//mov rax, 0x124569EA4125D98
				rdx ^= rax; 		//xor rdx, rax
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x5; 		//shr rax, 0x05
				rax ^= rdx; 		//xor rax, rdx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0xA; 		//shr rcx, 0x0A
				rcx ^= rax; 		//xor rcx, rax
				rdx = rcx; 		//mov rdx, rcx
				rdx >>= 0x14; 		//shr rdx, 0x14
				rdx ^= rcx; 		//xor rdx, rcx
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x28; 		//shr rax, 0x28
				rax ^= rdx; 		//xor rax, rdx
				rdx = rax; 		//mov rdx, rax
				rdx >>= 0x1A; 		//shr rdx, 0x1A
				rdx ^= rax; 		//xor rdx, rax
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x34; 		//shr rax, 0x34
				rcx = 0; 		//and rcx, 0xFFFFFFFFC0000000
				rdx ^= rax; 		//xor rdx, rax
				rcx = _rotl64(rcx, 0x10); 		//rol rcx, 0x10
				rcx ^= r10; 		//xor rcx, r10
				rcx = ~rcx; 		//not rcx
				rdx *= g_vm->read<uintptr_t>(rcx + 0x17); 		//imul rdx, [rcx+0x17]
				rdx ^= r11; 		//xor rdx, r11
				rax = 0xD83F30F92C64DF4F; 		//mov rax, 0xD83F30F92C64DF4F
				rdx ^= rax; 		//xor rdx, rax
				rax = 0xB69AFD2628432A9D; 		//mov rax, 0xB69AFD2628432A9D
				rdx *= rax; 		//imul rdx, rax
				return rdx;
			}
			case 14:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD59A04D); 		//mov r10, [0x00000000086DA3D3]
				r15 = g_vm->m_base_address + 0x5113; 		//lea r15, [0xFFFFFFFFFB145481]
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x1B; 		//shr rax, 0x1B
				rdx ^= rax; 		//xor rdx, rax
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x36; 		//shr rax, 0x36
				rcx = 0; 		//and rcx, 0xFFFFFFFFC0000000
				rdx ^= rax; 		//xor rdx, rax
				rcx = _rotl64(rcx, 0x10); 		//rol rcx, 0x10
				rcx ^= r10; 		//xor rcx, r10
				rcx = ~rcx; 		//not rcx
				rdx *= g_vm->read<uintptr_t>(rcx + 0x17); 		//imul rdx, [rcx+0x17]
				rax = 0xDC4274449EFE767B; 		//mov rax, 0xDC4274449EFE767B
				rdx ^= rax; 		//xor rdx, rax
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x6; 		//shr rax, 0x06
				rax ^= rdx; 		//xor rax, rdx
				rcx = rax; 		//mov rcx, rax
				rcx >>= 0xC; 		//shr rcx, 0x0C
				rcx ^= rax; 		//xor rcx, rax
				rdx = rcx; 		//mov rdx, rcx
				rdx >>= 0x18; 		//shr rdx, 0x18
				rdx ^= rcx; 		//xor rdx, rcx
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x30; 		//shr rax, 0x30
				rdx ^= rax; 		//xor rdx, rax
				rax = r15; 		//mov rax, r15
				rax ^= r11; 		//xor rax, r11
				rdx -= rax; 		//sub rdx, rax
				rax = 0x4480AA60A21867F9; 		//mov rax, 0x4480AA60A21867F9
				rdx *= rax; 		//imul rdx, rax
				rax = g_vm->m_base_address + 0xD03A; 		//lea rax, [0xFFFFFFFFFB14D086]
				rax += r11; 		//add rax, r11
				rdx += rax; 		//add rdx, rax
				return rdx;
			}
			case 15:
			{
				r10 = g_vm->read<uintptr_t>(g_vm->m_base_address + 0xD59A04D); 		//mov r10, [0x00000000086D9F43]
				r13 = g_vm->m_base_address + 0x642A39AC; 		//lea r13, [0x000000005F3E388F]
				r12 = g_vm->m_base_address + 0x6744783A; 		//lea r12, [0x000000006258770E]
				rdx += r11; 		//add rdx, r11
				rax = r11; 		//mov rax, r11
				rax = ~rax; 		//not rax
				rax ^= r13; 		//xor rax, r13
				rdx -= rax; 		//sub rdx, rax
				rdx ^= r12; 		//xor rdx, r12
				rdx ^= r11; 		//xor rdx, r11
				rax = 0; 		//and rax, 0xFFFFFFFFC0000000
				rax = _rotl64(rax, 0x10); 		//rol rax, 0x10
				rax ^= r10; 		//xor rax, r10
				rax = ~rax; 		//not rax
				rdx *= g_vm->read<uintptr_t>(rax + 0x17); 		//imul rdx, [rax+0x17]
				rax = 0x54750E0E4638841A; 		//mov rax, 0x54750E0E4638841A
				rdx += rax; 		//add rdx, rax
				rax = 0x17257FE07A931EB4; 		//mov rax, 0x17257FE07A931EB4
				rdx ^= rax; 		//xor rdx, rax
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x4; 		//shr rax, 0x04
				rdx ^= rax; 		//xor rdx, rax
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x8; 		//shr rax, 0x08
				rdx ^= rax; 		//xor rdx, rax
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x10; 		//shr rax, 0x10
				rdx ^= rax; 		//xor rdx, rax
				rax = rdx; 		//mov rax, rdx
				rax >>= 0x20; 		//shr rax, 0x20
				rdx ^= rax; 		//xor rdx, rax
				rax = 0x7493CCED6314B08B; 		//mov rax, 0x7493CCED6314B08B
				rdx *= rax; 		//imul rdx, rax
				return rdx;
			}
			}
		}
		uint16_t get_bone_index(uint32_t bone_index)
		{
			const uint64_t mb = g_vm->m_base_address;
			uint64_t rax = mb, rbx = mb, rcx = mb, rdx = mb, rdi = mb, rsi = mb, r8 = mb, r9 = mb, r10 = mb, r11 = mb, r12 = mb, r13 = mb, r14 = mb, r15 = mb;
			rbx = bone_index;
			rcx = rbx * 0x13C8;
			rax = 0xCB182C584BD5193; 		//mov rax, 0xCB182C584BD5193
			r11 = g_vm->m_base_address; 		//lea r11, [0xFFFFFFFFFC480421]
			rax = _umul128(rax, rcx, (uintptr_t*)&rdx); 		//mul rcx
			rax = rcx; 		//mov rax, rcx
			r10 = 0xD6FB75C08B670E5B; 		//mov r10, 0xD6FB75C08B670E5B
			rax -= rdx; 		//sub rax, rdx
			rax >>= 0x1; 		//shr rax, 0x01
			rax += rdx; 		//add rax, rdx
			rax >>= 0xC; 		//shr rax, 0x0C
			rax = rax * 0x1E7D; 		//imul rax, rax, 0x1E7D
			rcx -= rax; 		//sub rcx, rax
			rax = 0x4078E2A8FCDA18EF; 		//mov rax, 0x4078E2A8FCDA18EF
			r8 = rcx * 0x1E7D; 		//imul r8, rcx, 0x1E7D
			rax = _umul128(rax, r8, (uintptr_t*)&rdx); 		//mul r8
			rdx >>= 0xB; 		//shr rdx, 0x0B
			rax = rdx * 0x1FC4; 		//imul rax, rdx, 0x1FC4
			r8 -= rax; 		//sub r8, rax
			rax = 0xF0F0F0F0F0F0F0F1; 		//mov rax, 0xF0F0F0F0F0F0F0F1
			rax = _umul128(rax, r8, (uintptr_t*)&rdx); 		//mul r8
			rax = 0x624DD2F1A9FBE77; 		//mov rax, 0x624DD2F1A9FBE77
			rdx >>= 0x6; 		//shr rdx, 0x06
			rcx = rdx * 0x44; 		//imul rcx, rdx, 0x44
			rax = _umul128(rax, r8, (uintptr_t*)&rdx); 		//mul r8
			rax = r8; 		//mov rax, r8
			rax -= rdx; 		//sub rax, rdx
			rax >>= 0x1; 		//shr rax, 0x01
			rax += rdx; 		//add rax, rdx
			rax >>= 0x6; 		//shr rax, 0x06
			rcx += rax; 		//add rcx, rax
			rax = rcx * 0xFA; 		//imul rax, rcx, 0xFA
			rcx = r8 * 0xFC; 		//imul rcx, r8, 0xFC
			rcx -= rax; 		//sub rcx, rax
			rax = g_vm->read<uint16_t>(rcx + r11 * 1 + 0xC1F5380); 		//movzx eax, word ptr [rcx+r11*1+0xC1F5380]
			r8 = rax * 0x13C8; 		//imul r8, rax, 0x13C8
			rax = r10; 		//mov rax, r10
			rax = _umul128(rax, r8, (uintptr_t*)&rdx); 		//mul r8
			rax = r10; 		//mov rax, r10
			rdx >>= 0xD; 		//shr rdx, 0x0D
			rcx = rdx * 0x261B; 		//imul rcx, rdx, 0x261B
			r8 -= rcx; 		//sub r8, rcx
			r9 = r8 * 0x2F75; 		//imul r9, r8, 0x2F75
			rax = _umul128(rax, r9, (uintptr_t*)&rdx); 		//mul r9
			rdx >>= 0xD; 		//shr rdx, 0x0D
			rax = rdx * 0x261B; 		//imul rax, rdx, 0x261B
			r9 -= rax; 		//sub r9, rax
			rax = 0x8FB823EE08FB823F; 		//mov rax, 0x8FB823EE08FB823F
			rax = _umul128(rax, r9, (uintptr_t*)&rdx); 		//mul r9
			rax = 0x579D6EE340579D6F; 		//mov rax, 0x579D6EE340579D6F
			rdx >>= 0x5; 		//shr rdx, 0x05
			rcx = rdx * 0x39; 		//imul rcx, rdx, 0x39
			rax = _umul128(rax, r9, (uintptr_t*)&rdx); 		//mul r9
			rdx >>= 0x6; 		//shr rdx, 0x06
			rcx += rdx; 		//add rcx, rdx
			rax = rcx * 0x176; 		//imul rax, rcx, 0x176
			rcx = r9 * 0x178; 		//imul rcx, r9, 0x178
			rcx -= rax; 		//sub rcx, rax
			r14 = g_vm->read<uint16_t>(rcx + r11 * 1 + 0xC1F95F0); 		//movsx r14d, word ptr [rcx+r11*1+0xC1F95F0]
			return r14;
		}



	namespace offsets {

		constexpr auto timestamp = 0x6A3B43E6;
		constexpr auto ref_def_ptr = 0xE736F68;
		constexpr auto name_array = 0xE8BD280;
		constexpr auto name_array_pos = 0x3038;
		constexpr auto name_array_size = 0xD0;
		constexpr auto loot_ptr = 0xF8C7420;
		constexpr auto loot_size = 0x850;
		constexpr auto loot_pos = 0x178;
		constexpr auto loot_valid1 = 0x83C;
		constexpr auto loot_valid2 = 0x848;
		constexpr auto camera_base = 0xE33AE80;
		constexpr auto camera_pos = 0x1F4;
		constexpr auto local_index = 0xC9F70;
		constexpr auto local_index_pos = 0x418;
		constexpr auto game_mode = 0xD9A2BE8;
		constexpr auto weapon_definitions = 0xE51DCE0;
		constexpr auto distribute = 0xB03DAE0;
		constexpr auto visible_bits = 0x961F4;
		constexpr auto scoreboard = 0x15F538;
		constexpr auto scoreboardsize = 0x80;

		namespace bone {
			constexpr auto bone_base = 0x96300;
			constexpr auto size = 0x1D0;
			constexpr auto offset = 0xE0;
		};

		namespace player {
			constexpr auto size = 0x2760;
			constexpr auto valid = 0x200C;
			constexpr auto pos = 0x26B0;
			constexpr auto team = 0x1187;
			constexpr auto stance = 0x701;
			constexpr auto weapon_index = 0x1FB0;
		};

		namespace entity {
			constexpr auto predictedPlayerEntity = 0x3A400;
			constexpr auto o_local_index = 0x530;
			constexpr auto o_entity_type = o_local_index + 0x8;
			constexpr auto o_local_valid = 0x84;
			constexpr auto o_local_stance = 0x53C;
			constexpr auto o_local_pos = 0x554;
			constexpr auto o_local_size = 0x710;
		};


	}

}} // namespace decryptions::xbox
