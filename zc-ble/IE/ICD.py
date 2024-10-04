class ICD:
    def __init__(self):
        self.icd_name_list = [
            "화기조준경 영상 종류",
            "화기조준경 영상 출력",
            "무선 채널 설정 모드",
            "60GHz 무선 채널 정보",
            "화기조준경 운용모드 상태",
            "무선영상 송신기 운용모드 상태",
            "무선영상 수신기 운용모드 상태",
            "무선영상 송신기 IMU Output Type",
            "무선영상 송신기 IMU 보정",
            "무선영상 수신기 IMU Output Type",
            "무선영상 수신기 IMU 보정",
            "화기조준경 배터리 잔량",
            "무선영상 송신기 배터리 잔량",
            "화기조준경 IR 모듈 상태",
            "화기조준경 EO 모듈 상태",
            "무선영상 송신기 모뎀 상태",
            "무선영상 수신기 모뎀 상태",
            "무선영상 송신기 IMU 상태",
            "무선영상 수신기 IMU 상태",
            "BLE 상태",
            "무선영상 송신기 IMU Data 1",
            "무선영상 송신기 IMU Data 2",
            "무선영상 송신기 IMU Data 3",
            "무선영상 송신기 IMU Data 4",
            "무선영상 송신기 IMU Data 5",
            "무선영상 송신기 IMU Checksum",
            "무선영상 수신기 IMU Data 1",
            "무선영상 수신기 IMU Data 2",
            "무선영상 수신기 IMU Data 3",
            "무선영상 수신기 IMU Data 4",
            "무선영상 수신기 IMU Data 5",
            "무선영상 수신기 IMU Checksum",
            "ZCH USB", # HBLE
            "ZCH Vendor ID",
            "ZCH Product ID",
            "ZCH Channel",
            "ZCH PPID",
            "ZCH Device ID",
            "ZCH Format",
            "ZCH Index",
            "ZCH TRT",
            "ZCH ACK",
            "ZCH PPC",
            "ZCH RUN",
            "ZCH TXID",
            "ZCH RXID",
            "ZCH Count",
            "ZCD USB", # DBLE
            "ZCD PPID",
            "ZCD Device ID",
            "ZCD Format",
            "ZCD Index",
            "ZCD FPS",
            "ZCD TRT",
            "ZCD ACK",
            "ZCD PPC",
            "ZCD RUN",
            "ZCD ITF",
            "ZCD TXID",
            "ZCD RXID",
            "ZCD DestID_ERR_CNT",
            "ZCD DestID_ERR_CNT_DIFF",
            "ZCD PHY_RX_FRAME_CNT",
            "ZCD PHY_RX_FRAME_CNT_DIFF",
            "ZCD MFIR",
            "ZCD Count",
        ]

        self.icd_type_list = [
            "화기조준경 영상 종류",
            "화기조준경 영상 출력",
            "무선 채널 설정 모드",
            "60GHz 무선 채널 정보",
            "화기조준경 운용모드 상태",
            "무선영상 송신기 운용모드 상태",
            "무선영상 수신기 운용모드 상태",
            "무선영상 송신기 IMU Output Type",
            "무선영상 송신기 IMU 보정",
            "무선영상 수신기 IMU Output Type",
            "무선영상 수신기 IMU 보정",
            "d",
            "d",
            "화기조준경 IR 모듈 상태",
            "화기조준경 EO 모듈 상태",
            "무선영상 송신기 모뎀 상태",
            "무선영상 수신기 모뎀 상태",
            "무선영상 송신기 IMU 상태",
            "무선영상 수신기 IMU 상태",
            "BLE 상태",
            'x',
            'x',
            'x',
            'x',
            'x',
            'x',
            'x',
            'x',
            'x',
            'x',
            'x',
            'x',
            'd', # HBLE
            'x',
            'x',
            'BND',
            'x',
            'x',
            'FMT',
            'IDX',
            'TRT',
            'ACK',
            'PPC',
            'RUN',
            'x',
            'x',
            'd',
            'd', # DBLE
            'x',
            'x',
            'FMT',
            'IDX',
            'FPS',
            'TRT',
            'ACK',
            'PPC',
            'RUN',
            'ITF',
            'x',
            'x',
            'd',
            'd',
            'd',
            'd',
            "MFIR",
            'd',
        ]

        self.icd_i2c_list = [
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00, 0x00,
            0x00, 0x00,
            0x00, 0x00,
            0x00, 0x00,
            0x00, 0x00,
            0x00, 0x00,
            0x00, 0x00,
            0x00, 0x00,
            0x00, 0x00,
            0x00, 0x00,
            0x00, 0x00,
            0x00, 0x00,
            0x00, # HBLE
            0x00, 0x00,
            0x00, 0x00,
            0x00,
            0x00, 0x00,
            0x00, 0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00,
            0x00, # DBLE
            0x00, 0x00,
            0x00, 0x00,
            0x00,
            0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00,
        ]
        self.icd_list = [
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x0000,
            0x0000,
            0x0000,
            0x0000,
            0x0000,
            0x0000,
            0x0000,
            0x0000,
            0x0000,
            0x0000,
            0x0000,
            0x0000,
            0x00, # HBLE
            0x0000,
            0x0000,
            0x00,
            0x0000,
            0x0000,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00000000,
            0x00000000,
            0x0000,
            0x00, # DBLE
            0x0000,
            0x0000,
            0x00,
            0x00,
            0x00000000,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00000000,
            0x00000000,
            0x00000000,
            0x0000,
            0x00000000,
            0x0000,
            0x00000000,
            0x0000,
        ]

    def set_icd(self):
        self.icd_list[0] = self.icd_i2c_list[0]
        self.icd_list[1] = self.icd_i2c_list[1]
        self.icd_list[2] = self.icd_i2c_list[2]
        self.icd_list[3] = self.icd_i2c_list[3]
        self.icd_list[4] = self.icd_i2c_list[4]
        self.icd_list[5] = self.icd_i2c_list[5]
        self.icd_list[6] = self.icd_i2c_list[6]
        self.icd_list[7] = self.icd_i2c_list[7]
        self.icd_list[8] = self.icd_i2c_list[8]
        self.icd_list[9] = self.icd_i2c_list[9]
        self.icd_list[10] = self.icd_i2c_list[10]
        self.icd_list[11] = self.icd_i2c_list[11]
        self.icd_list[12] = self.icd_i2c_list[12]
        self.icd_list[13] = self.icd_i2c_list[13]
        self.icd_list[14] = self.icd_i2c_list[14]
        self.icd_list[15] = self.icd_i2c_list[15]
        self.icd_list[16] = self.icd_i2c_list[16]
        self.icd_list[17] = self.icd_i2c_list[17]
        self.icd_list[18] = self.icd_i2c_list[18]
        self.icd_list[19] = self.icd_i2c_list[19]
        self.icd_list[20] = (self.icd_i2c_list[20] << 8) | self.icd_i2c_list[21]
        self.icd_list[21] = (self.icd_i2c_list[22] << 8) | self.icd_i2c_list[23]
        self.icd_list[22] = (self.icd_i2c_list[24] << 8) | self.icd_i2c_list[25]
        self.icd_list[23] = (self.icd_i2c_list[26] << 8) | self.icd_i2c_list[27]
        self.icd_list[24] = (self.icd_i2c_list[28] << 8) | self.icd_i2c_list[29]
        self.icd_list[25] = (self.icd_i2c_list[30] << 8) | self.icd_i2c_list[31]
        self.icd_list[26] = (self.icd_i2c_list[32] << 8) | self.icd_i2c_list[33]
        self.icd_list[27] = (self.icd_i2c_list[34] << 8) | self.icd_i2c_list[35]
        self.icd_list[28] = (self.icd_i2c_list[36] << 8) | self.icd_i2c_list[37]
        self.icd_list[29] = (self.icd_i2c_list[38] << 8) | self.icd_i2c_list[39]
        self.icd_list[30] = (self.icd_i2c_list[40] << 8) | self.icd_i2c_list[41]
        self.icd_list[31] = (self.icd_i2c_list[42] << 8) | self.icd_i2c_list[43]
        self.icd_list[32] = self.icd_i2c_list[44] # HBLE
        self.icd_list[33] = (self.icd_i2c_list[45] << 8) | self.icd_i2c_list[46]
        self.icd_list[34] = (self.icd_i2c_list[47] << 8) | self.icd_i2c_list[48]
        self.icd_list[35] = self.icd_i2c_list[49]
        self.icd_list[36] = (self.icd_i2c_list[50] << 8) | self.icd_i2c_list[51]
        self.icd_list[37] = (self.icd_i2c_list[52] << 8) | self.icd_i2c_list[53]
        self.icd_list[38] = self.icd_i2c_list[54]
        self.icd_list[39] = self.icd_i2c_list[55]
        self.icd_list[40] = self.icd_i2c_list[56]
        self.icd_list[41] = self.icd_i2c_list[57]
        self.icd_list[42] = self.icd_i2c_list[58]
        self.icd_list[43] = self.icd_i2c_list[59]
        self.icd_list[44] = (self.icd_i2c_list[60] << 24) | (self.icd_i2c_list[61] << 16) | (self.icd_i2c_list[62] << 8) | self.icd_i2c_list[63]
        self.icd_list[45] = (self.icd_i2c_list[64] << 24) | (self.icd_i2c_list[65] << 16) | (self.icd_i2c_list[66] << 8) | self.icd_i2c_list[67]
        self.icd_list[46] = (self.icd_i2c_list[68] << 8) | self.icd_i2c_list[69]
        self.icd_list[47] = self.icd_i2c_list[70] # DBLE
        self.icd_list[48] = (self.icd_i2c_list[71] << 8) | self.icd_i2c_list[72]
        self.icd_list[49] = (self.icd_i2c_list[73] << 8) | self.icd_i2c_list[74]
        self.icd_list[50] = self.icd_i2c_list[75]
        self.icd_list[51] = self.icd_i2c_list[76]
        self.icd_list[52] = (self.icd_i2c_list[77] << 24) | (self.icd_i2c_list[78] << 16) | (self.icd_i2c_list[79] << 8) | self.icd_i2c_list[80]
        self.icd_list[53] = self.icd_i2c_list[81]
        self.icd_list[54] = self.icd_i2c_list[82]
        self.icd_list[55] = self.icd_i2c_list[83]
        self.icd_list[56] = self.icd_i2c_list[84]
        self.icd_list[57] = self.icd_i2c_list[85]
        self.icd_list[58] = (self.icd_i2c_list[86] << 24) | (self.icd_i2c_list[87] << 16) | (self.icd_i2c_list[88] << 8) | self.icd_i2c_list[89]
        self.icd_list[59] = (self.icd_i2c_list[90] << 24) | (self.icd_i2c_list[91] << 16) | (self.icd_i2c_list[92] << 8) | self.icd_i2c_list[93]
        self.icd_list[60] = (self.icd_i2c_list[94] << 24) | (self.icd_i2c_list[95] << 16) | (self.icd_i2c_list[96] << 8) | self.icd_i2c_list[97]
        self.icd_list[61] = (self.icd_i2c_list[98] << 8) | self.icd_i2c_list[99]
        self.icd_list[62] = (self.icd_i2c_list[100] << 24) | (self.icd_i2c_list[101] << 16) | (self.icd_i2c_list[102] << 8) | self.icd_i2c_list[103]
        self.icd_list[63] = (self.icd_i2c_list[104] << 8) | self.icd_i2c_list[105]
        self.icd_list[64] = (self.icd_i2c_list[106] << 24) | (self.icd_i2c_list[107] << 16) | (self.icd_i2c_list[108] << 8) | self.icd_i2c_list[109]
        self.icd_list[65] = (self.icd_i2c_list[110] << 8) | self.icd_i2c_list[111]