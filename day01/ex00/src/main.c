#include "hal.h"

// here we wait artificially by using a dead loop
// the result is approximative, but the concept is simple
// each instructions the CPU executes has a latency of at least 1 cycle
// since the F_CPU is configured to run at 16Mhz that means that we have
// 16 millions of operations that can be executed per seconds.
// so we take F_CPU and divide it by a slice to approximately waste
// 1/2hz worth of time doing nothing
static void wait(volatile u64 cycles) {
        volatile u64 i = 0;
        while (i < cycles) {
                i++;
        }
}

GpioPin led;

int     main() {

        // this is my simple hal library (Hardware Abstraction Layer)
        // this library abstracts common operations in a hardware agnostic
        gpio_init(&led, GPIO_PORTB, PB0);
        gpio_set_output(&led);

        loop {
                // 160 000 since there is about 50 instructions in the dissasembly that is related to the loop
                // and that on average each instructions is about 1/2 cycles this is about [1hz-1.2hz]
                // at least that's my calculation of it.
                wait(F_CPU / 100);
                gpio_toggle(&led);
        }
}

// firmware.elf:     file format elf32-avr

// 00000090 <main>:
//   90:   cf 93           push    r28
//   92:   df 93           push    r29
//   ................................... ; skiped
//  10a:   0e 94 b7 00     call    0x16e   ; 0x16e <__cmpdi2>                             ; 32 instructions
//  10e:   78 f0           brcs    .+30            ; 0x12e <main+0x9e>
//  110:   e0 91 00 01     lds     r30, 0x0100     ; 0x800100 <__DATA_REGION_ORIGIN__>
//  114:   f0 91 01 01     lds     r31, 0x0101     ; 0x800101 <__DATA_REGION_ORIGIN__+0x1>
//  118:   80 81           ld      r24, Z
//  11a:   90 91 06 01     lds     r25, 0x0106     ; 0x800106 <__DATA_REGION_ORIGIN__+0x6>
//  11e:   94 01           movw    r18, r8
//  120:   01 c0           rjmp    .+2             ; 0x124 <main+0x94>
//  122:   22 0f           add     r18, r18
//  124:   9a 95           dec     r25
//  126:   ea f7           brpl    .-6             ; 0x122 <main+0x92>
//  128:   82 27           eor     r24, r18
//  12a:   80 83           st      Z, r24
//  12c:   d3 cf           rjmp    .-90            ; 0xd4 <main+0x44>
//  12e:   29 81           ldd     r18, Y+1        ; 0x01
//  130:   3a 81           ldd     r19, Y+2        ; 0x02
//  132:   4b 81           ldd     r20, Y+3        ; 0x03
//  134:   5c 81           ldd     r21, Y+4        ; 0x04
//  136:   6d 81           ldd     r22, Y+5        ; 0x05
//  138:   7e 81           ldd     r23, Y+6        ; 0x06
//  13a:   8f 81           ldd     r24, Y+7        ; 0x07
//  13c:   98 85           ldd     r25, Y+8        ; 0x08
//  13e:   a1 e0           ldi     r26, 0x01       ; 1
//  140:   0e 94 ab 00     call    0x156   ; 0x156 <__adddi3_s8>
//  144:   29 83           std     Y+1, r18        ; 0x01
//  146:   3a 83           std     Y+2, r19        ; 0x02
//  148:   4b 83           std     Y+3, r20        ; 0x03
//  14a:   5c 83           std     Y+4, r21        ; 0x04
//  14c:   6d 83           std     Y+5, r22        ; 0x05
//  14e:   7e 83           std     Y+6, r23        ; 0x06
//  150:   8f 83           std     Y+7, r24        ; 0x07
//  152:   98 87           std     Y+8, r25        ; 0x08
//  154:   c7 cf           rjmp    .-114           ; 0xe4 <main+0x54>

// 00000156 <__adddi3_s8>:                                                                ; this is 12 instructions
//  156:   00 24           eor     r0, r0
//  158:   a7 fd           sbrc    r26, 7
//  15a:   00 94           com     r0
//  15c:   2a 0f           add     r18, r26
//  15e:   30 1d           adc     r19, r0
//  160:   40 1d           adc     r20, r0
//  162:   50 1d           adc     r21, r0
//  164:   60 1d           adc     r22, r0
//  166:   70 1d           adc     r23, r0
//  168:   80 1d           adc     r24, r0
//  16a:   90 1d           adc     r25, r0
//  16c:   08 95           ret

// 0000016e <__cmpdi2>:                                                                   ; this is 9 instructions
//  16e:   2a 15           cp      r18, r10
//  170:   3b 05           cpc     r19, r11
//  172:   4c 05           cpc     r20, r12
//  174:   5d 05           cpc     r21, r13
//  176:   6e 05           cpc     r22, r14
//  178:   7f 05           cpc     r23, r15
//  17a:   80 07           cpc     r24, r16
//  17c:   91 07           cpc     r25, r17
//  17e:   08 95           ret

// 00000180 <_exit>:
//  180:   f8 94           cli

// 00000182 <__stop_program>:
//  182:   ff cf           rjmp    .-2             ; 0x182 <__stop_program>
