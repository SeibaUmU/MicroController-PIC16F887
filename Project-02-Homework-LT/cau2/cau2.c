unsigned char Led7Seg_CC[10] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};

void main() {
    unsigned char count_val;     // Biến lưu giá trị thực tế của Timer0
    unsigned char display_val;   // Biến giá trị cần hiển thị (25 -> 0)

    // 1. Cấu hình Port
    ANSEL = 0; ANSELH = 0;       // Cấu hình Digital (quan trọng với RA4)
    TRISB = 0x00;                // PORTB là Output (Hàng chục)
    TRISD = 0x00;                // PORTD là Output (Hàng đơn vị)
    TRISA.F4 = 1;                // RA4/T0CKI là Input (Nhận xung đếm)

    PORTB = 0; PORTD = 0;        // Tắt LED ban đầu

    // 2. Cấu hình Timer0 làm Counter (Đếm xung ngoài)
    // OPTION_REG (Thanh ghi quy định chế độ Timer0)
    // Bit 5 (T0CS): 1 = Counter mode (xung từ chân T0CKI/RA4)
    // Bit 4 (T0SE): 1 = Đếm cạnh xuống (nhấn nút), 0 = Đếm cạnh lên (nhả nút)
    // Bit 3 (PSA):  1 = Không dùng bộ chia tần (Prescaler) cho Timer0
    
    /* Lưu ý: Để đếm chính xác từng nhịp nhấn, ta nên để PSA=1 (tỉ lệ 1:1)
       Code chuẩn: OPTION_REG = 0b00101000; (T0CS=1, T0SE=0, PSA=0 -> Prescaler gán Timer0)
       Sửa lại: Để tỉ lệ 1:1 thì PSA = 1 (gán Prescaler cho WDT).
       Vậy: OPTION_REG.T0CS = 1;
            OPTION_REG.T0SE = 1; (Đếm khi nhấn - High xuống Low)
            OPTION_REG.PSA = 1;  (Tỉ lệ 1:1)
    */
    OPTION_REG = 0b00111000; // T0CS=1 (Counter), T0SE=1 (Edge), PSA=1 (No Prescaler)

    TMR0 = 0; // Xóa thanh ghi đếm về 0

    while(1) {
        // Đọc giá trị từ thanh ghi phần cứng TMR0
        count_val = TMR0;

        // Xử lý giới hạn đếm (Nếu đếm quá 25 xung thì reset)
        if (count_val > 25) {
            TMR0 = 0;       // Reset phần cứng
            count_val = 0;  // Reset biến
        }

        // Tính toán đếm lùi: Hiển thị = 25 - Đã_đếm
        display_val = 25 - count_val;

        // Hiển thị ra LED (Không cần quét vì mỗi LED nối 1 Port riêng)
        PORTB = Led7Seg[display_val / 10]; // Hàng chục
        PORTD = Led7Seg[display_val % 10]; // Hàng đơn vị
        
        // Không cần Delay vì Counter đếm phần cứng độc lập với hiển thị
    }
}

