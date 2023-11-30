#ifndef _GROUND_COMMAND_LIST_H_
#define _GROUND_COMMAND_LIST_H_

unsigned char ground_message_lengths[255] = { 
	       0, /* command   0 */	
        0, /* command   1 */
        0, /* command   2 */
        0, /* command   3 */
        5, /* command   4 : TC_SET_EXPOSURE_TIME ci [OK] */
        0, /* command   5 */
        0, /* command   6 */
        0, /* command   7 */
        0, /* command   8 */
        0, /* command   9 */
        0, /* command  10 */
        0, /* command  11 */
        0, /* command  12 */
        0, /* command  13 */
        0, /* command  14 */
        0, /* command  15 */
        0, /* command  16 */
        0, /* command  17 */
        0, /* command  18 */
        0, /* command  19 */
       13, /* command  20 : TC_SET_DISPLAY_STRETCH [OK] */
        0, /* command  21 */
        0, /* command  22 */
        0, /* command  23 */
        0, /* command  24 */
        2, /* command  25 : TC_SET_GUIDER_SUBTRACT [OK] */
        0, /* command  26 */
        0, /* command  27 */
        2, /* command  28 : TC_SET_DISPLAY_MODE [OK] */
        0, /* command  29 */
        0, /* command  30 */
        0, /* command  31 */
        0, /* command  32 */
        0, /* command  33 */
        0, /* command  34 */
        0, /* command  35 */
        0, /* command  36 */
        0, /* command  37 */
        0, /* command  38 */
        0, /* command  39 */
        2, /* command  40 : TC_SET_GUIDER_MODE [OK] */
       13, /* command  41 : TC_CONVERT_NEXT_TARGET_TO_NEXT_STAR [OK] */
        1, /* command  42 : TC_CONVERT_NEXT_TARGET_TO_STAR_SKY [OK] */
        2, /* command  43 : TC_SELECT_CENTROIDING_ALGORITHM [OK] */
       13, /* command  44 : TC_ADJUST_POINTING [OK] */
        2, /* command  45 : TC_SET_GUIDER_MODE_ACQUIRE_LIMITED [OK] */
       10, /* command  46 : TC_ADJUST_SINGLE_STAR [OK] */
       33, /* command  47 : TC_SET_TARGET_POSITIONS [OK] */
        9, /* command  48 : TC_SWITCH_STAR [OK] */
        3, /* command  49 : TC_LOAD_TARGET [OK] */
        2, /* command  50 : TC_SWITCH_TO_NEXT [OK] */
        0, /* command  51 */
        0, /* command  52 */
        0, /* command  53 */
        2, /* command  54 : TC_CHANGE_USED_STARS [OK] */
        6, /* command  55 : TC_ADJUST_SIGMA_MINMAX [OK] */
        0, /* command  56 */
        0, /* command  57 */
        0, /* command  58 */
        0, /* command  59 */
        0, /* command  60 */
        0, /* command  61 */
        0, /* command  62 */
        0, /* command  63 */
        0, /* command  64 */
        0, /* command  65 */
        0, /* command  66 */
        0, /* command  67 */
        0, /* command  68 */
        0, /* command  69 */
        0, /* command  70 */
        0, /* command  71 */
        0, /* command  72 */
        0, /* command  73 */
        0, /* command  74 */
        0, /* command  75 */
        0, /* command  76 */
        0, /* command  77 */
        0, /* command  78 */
        0, /* command  79 */
        3, /* command  80 : TC_SET_TTL_ON [OK]*/
        2, /* command  81 : TC_SET_TTL__OFF* [OK] /
        0, /* command  82 */
        0, /* command  83 */
        0, /* command  84 */
        0, /* command  85 */
        0, /* command  86 */
        0, /* command  87 */
        0, /* command  88 */
        0, /* command  89 */
        3, /* command  90 : TC_SET_IMAGE_TAG [OK] */
        3, /* command  91 : TC_SET_IMAGE_SAVE_PERIOD [OK] */
        2, /* command  92 : TC_REFINE_TARGETS [OK] */
        2, /* command  93 : TC_COLLECT_MIN_IMAGE [OK] */
        3, /* command  94 : TC_DITHER [OK] */
        0, /* command  95 */
        0, /* command  96 */
        0, /* command  97 */
        0, /* command  98 */
        2, /* command  99 : TC_CPU_POWER_ID */
        1, /* command 100 : TC_MASK_STAGE_HOME [OK] */
        5, /* command 101 : TC_MASK_STAGE_POSITION [OK]*/
        0, /* command 102 */
        0, /* command 103 */
        0, /* command 104 */
        0, /* command 105 */
        0, /* command 106 */
        0, /* command 107 */
        0, /* command 108 */
        0, /* command 109 */
        1, /* command 110 : TC_TIPTILT_STAGE_HOME [OK] */
        5, /* command 111 : TC_TIPTILT_STAGE_POSITION [OK] */
        0, /* command 112 */
        0, /* command 113 : TC_ROTATION_CENTER */
        0, /* command 114 : TC_PLATE_SCALE */
        0, /* command 115 : TC_TRACKSTAR_NUMBER */
        0, /* command 116 : TC_STACK_SIZE_ID*/
        5, /* command 117 : TC_SOLVE_FIELD [OK] */
        0, /* command 118 */
        0, /* command 119 */
        1, /* command 120 : TC_TIPTILT_STAGE_2_HOME [OK] */
        5, /* command 121 : TC_TIPTILT_STAGE_2_POSITION [OK] */
        0, /* command 122 */
        0, /* command 123 */
        0, /* command 124 */
        0, /* command 125 */
        0, /* command 126 */
        0, /* command 127 */
        0, /* command 128 */
        0, /* command 129 */
        1, /* command 130 : TC_TIPTILT_STAGE_3_HOME [OK] */
        5, /* command 131 : TC_TIPTILT_STAGE_3_POSITION [OK] */
        6, /* command 132 : TC_TIPTILT_FOCUS [OK] */
        1, /* command 133 : TC_TIPTILT_ABCMOVES*/
        0, /* command 134 */
        2, /* command 135 : TC_CAROUSEL_POS [OK] */
        1, /* command 136 : TC_NOMINAL_FOCUS [OK] */
        5, /* command 137 : TC_FOCUS_DELTA [OK] */
        0, /* command 138 */
        0, /* command 139 */
        2, /* command 140 : TC_SHUTDOWN_GUIDER [OK] */
        0, /* command 141 */
        0, /* command 142 */
        0, /* command 143 */
        0, /* command 144 */
        0, /* command 145 */
        0, /* command 146 */
        0, /* command 147 */
        0, /* command 148 */
        0, /* command 149 */
        2, /* command 150 : TC_STAR_BOX_CUT [OK] */
        0, /* command 151 */
        0, /* command 152 */
        0, /* command 153 */
        0, /* command 154 */
        0, /* command 155 */
        0, /* command 156 */
        0, /* command 157 */
        0, /* command 158 */
        0, /* command 159 */
        0, /* command 160 */
        0, /* command 161 */
        0, /* command 162 */
        0, /* command 163 */
        0, /* command 164 */
        0, /* command 165 */
        0, /* command 166 */
        0, /* command 167 */
        0, /* command 168 */
        0, /* command 169 */
        0, /* command 170 */
        0, /* command 171 */
        0, /* command 172 */
        0, /* command 173 */
        0, /* command 174 */
        0, /* command 175 */
        0, /* command 176 */
        0, /* command 177 */
        0, /* command 178 */
        0, /* command 179 */
        0, /* command 180 */
        0, /* command 181 */
        0, /* command 182 */
        0, /* command 183 */
        0, /* command 184 */
        0, /* command 185 */
        0, /* command 186 */
        0, /* command 187 */
        0, /* command 188 */
        0, /* command 189 */
        0, /* command 190 */
        0, /* command 191 */
        0, /* command 192 */
        0, /* command 193 */
        0, /* command 194 */
        0, /* command 195 */
        0, /* command 196 */
        0, /* command 197 */
        0, /* command 198 */
        0, /* command 199 */
        0, /* command 200 : TC_FIELD_SOLUTION [OK] - this is TM*/
        0, /* command 201 */
        0, /* command 202 */
        0, /* command 203 */
        0, /* command 204 */
        0, /* command 205 */
        0, /* command 206 */
        0, /* command 207 */
        0, /* command 208 */
        0, /* command 209 */
        0, /* command 210 */
        0, /* command 211 */
        0, /* command 212 */
        0, /* command 213 */
        0, /* command 214 */
        0, /* command 215 */
        0, /* command 216 */
        0, /* command 217 */
        0, /* command 218 */
        0, /* command 219 */
        0, /* command 220 */
        5, /* command 221 : TC_SET_FB_GAIN [OK] */
        3, /* command 222 : TC_SET_FB_SATVAL [OK] */
        5, /* command 223 : TC_SET_FB_THRESH [OK] */
        3, /* command 224 : TC_SET_FB_DISTOL [OK] */
        3, /* command 225 : TC_SET_FB_GRIDSIZE [OK] */
        3, /* command 226 : TC_SET_FB_CENBOX [OK] */
        3, /* command 227 : TC_SET_FB_APBOX [OK] */
        5, /* command 228 : TC_SET_FB_OFFSET [OK] */
        5, /* command 229 : TC_SET_FB_RN [OK] */
        1, /* command 230 : TC_RESET_FB [OK] */
        0, /* command 231 */
        0, /* command 232 */
        0, /* command 233 */
        0, /* command 234 */
        0, /* command 235 */
        0, /* command 236 */
        0, /* command 237 */
        0, /* command 238 */
        0, /* command 239 */
        0, /* command 240 */
        0, /* command 241 */
        0, /* command 242 */
        0, /* command 243 */
        0, /* command 244 */
        0, /* command 245 */
        0, /* command 246 */
        0, /* command 247 */
        0, /* command 248 : TC_STAGE_STATUS - this is TM */
        0, /* command 249 */
        0, /* command 250 */
        0, /* command 251 */
        0, /* command 252 */
        0, /* command 253 */
        0, /* command 254 */
        0 /* command 255 */
	     };


#endif