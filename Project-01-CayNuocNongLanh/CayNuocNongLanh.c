/*
  Nhom ?
  Thanh vien:
        Nguyen Manh Thang (code)
        Cao Quoc Trong (proteus)
        Huynh Huu Thien (Luu do giai thuat + so do khoi)
        Cao Huynh Phi (file bao cao)
  De tai: Cay nuoc nong lanh hien thi len LCD
  Tham khao: toshiba rwf-w1830uvbv(t) 650w

  Timeout 60s cho motor bom nuoc - cong suat bom nuoc tu binh 20l len binh trung gian 1l la 4-6W
  => mat 60s de bom day
*/
//====================== GLCD CONNECTIONS ======================//
sbit GLCD_DataPort at PORTD;

sbit GLCD_D7 at RD7_bit;
sbit GLCD_D6 at RD6_bit;
sbit GLCD_D5 at RD5_bit;
sbit GLCD_D4 at RD4_bit;
sbit GLCD_D3 at RD3_bit;
sbit GLCD_D2 at RD2_bit;
sbit GLCD_D1 at RD1_bit;
sbit GLCD_D0 at RD0_bit;

sbit GLCD_D7_Direction at TRISD7_bit;
sbit GLCD_D6_Direction at TRISD6_bit;
sbit GLCD_D5_Direction at TRISD5_bit;
sbit GLCD_D4_Direction at TRISD4_bit;
sbit GLCD_D3_Direction at TRISD3_bit;
sbit GLCD_D2_Direction at TRISD2_bit;
sbit GLCD_D1_Direction at TRISD1_bit;
sbit GLCD_D0_Direction at TRISD0_bit;

sbit GLCD_CS1 at RC5_bit;
sbit GLCD_CS2 at RC4_bit;
sbit GLCD_RS  at RB2_bit;
sbit GLCD_RW  at RB3_bit;
sbit GLCD_EN  at RC6_bit;
sbit GLCD_RST at RB5_bit;

sbit GLCD_CS1_Direction at TRISC5_bit;
sbit GLCD_CS2_Direction at TRISC4_bit;
sbit GLCD_RS_Direction  at TRISB2_bit;
sbit GLCD_RW_Direction  at TRISB3_bit;
sbit GLCD_EN_Direction  at TRISC6_bit;
sbit GLCD_RST_Direction at TRISB5_bit;
// End LCD module connections
#define temp_hot_default  80;
#define temp_cold_default  5;
unsigned char binhnuoc, nuocnong, nuoclanh, nhietdobinhnong, nhietdobinhlanh, temp_hot_lcd, temp_cold_lcd, temp_set_hot, temp_set_cold;
unsigned short mode = 0;
bit flag_ngat, flag_ngat2, flag_timer0;
bit pump_running;           // o danh dau motor dang chay
bit pump_timeout;           // Co bao timeout 10s
unsigned int timer0_count = 0;      // Bien dem cho do nhiet do (5s)
unsigned int pump_timer_count = 0;  // Bien dem cho timeout motor (10s)

// =============================================
//   Ham ve giao dien GLCD
// =============================================
void Draw_Interface() {
    char temp_str[5];
    unsigned char water_level;
    unsigned char bar_top;
    unsigned char bar_bottom;
    unsigned char bar_height;
    unsigned char fill_start;

    Glcd_Fill(0x00);
    Glcd_Set_Font(Font5x7, 5, 7, 32);

    // Thanh cot ben trai
    Glcd_Box(5, 10, 20, 55, 1);
    Glcd_Rectangle(6, 11, 19, 54, 0);

    // TTinh muc nuoc
    if (binhnuoc == 1) {
        water_level = 90;
    } else {
        water_level = 10;
    }

    // Ve muc nuoc
    bar_bottom = 53;
    bar_top = 12;
    bar_height = bar_bottom - bar_top;
    fill_start = bar_bottom - (water_level * bar_height / 100); 
    Glcd_Box(7, 12, 18, 53, 0);
    if(water_level > 0) {
        Glcd_Box(7, fill_start, 18, bar_bottom, 1);
    }
     // NEU (NEUTRAL)
    Glcd_Write_Text("Neu", 25, 2, 1);


    // HOT
    Glcd_Write_Text("Hot", 55, 2, 1);
    IntToStr(temp_hot_lcd, temp_str);
    Ltrim(temp_str);
    Glcd_Write_Text(temp_str, 55, 4, 1);
    Glcd_Write_Text("*C", 70, 4, 1);

    // COLD
    Glcd_Write_Text("Cold", 85, 2, 1);
    IntToStr(temp_cold_lcd, temp_str);
    Ltrim(temp_str);
    Glcd_Write_Text(temp_str, 85, 4, 1);
    Glcd_Write_Text("*C", 100, 4, 1);

    // Vien khung
    Glcd_Rectangle(0, 0, 127, 63, 1);
}


void interrupt(){
    if (INTF_bit) {
        Delay_ms(50);
        binhnuoc = PORTB.F0;

        // Neu binh day (RB0=1), tat motor va reset timeout
        if (binhnuoc == 1) {
            INTEDG_bit = 0;
            portc.f7 = 0;
            porte.f0 = 0;
            pump_running = 0;
            pump_timer_count = 0;  // Reset bien dem timeout
            pump_timeout = 0;       // Xoa co timeout
            flag_ngat = 1;          // Cap nhat hien thi 90%
        } else {
            INTEDG_bit = 1;
            // RB0=0: KHONG set flag_ngat, giu nguyen hien thi
        }

        INTF_bit = 0;
    }

    if (RBIF_bit) {
         flag_ngat2 = 1;
         if (portb.f4==0) nuocnong = 0; //muc 3/5
         else nuocnong = 1;

         if (portb.f5 == 0) nuoclanh = 0;
         else nuoclanh = 1;
         RBIF_bit = 0;
    }

    // Ngat Timer0
    if (TMR0IF_bit) {
        TMR0IF_bit = 0;
        TMR0 = 0;

        // dem nhiet do (5s = 76 lan)
        timer0_count++;
        if (timer0_count >= 76) {
            timer0_count = 0;
            flag_timer0 = 1;
        }

        // dem cho timeout motor (60s = 916 lan)
        if (pump_running) {
            pump_timer_count++;
            if (pump_timer_count >= 916) {  // 916 x 65.536ms = 60s
                pump_timer_count = 0;
                pump_timeout = 1;  // dat co timeout
                pump_running = 0;
                flag_ngat = 1;     // Bao cap nhat hien thi 10%
            }
        }
    }
}

void mucnuoc_binh(){
    unsigned char w, f;
    w = (binhnuoc) ? 90 : 10; //w la muc nuoc hien tai, 90% hoac 10%
    f = 53 - (w * 41 / 100); //f la vi tri bat dau ve muc nuoc tren GLCD
    Glcd_Box(7, 12, 18, 53, 0); //xoa muc nuoc cu
    if(w > 0) Glcd_Box(7, f, 18, 53, 1); //ve muc nuoc moi
}

int button_mode(){
    unsigned short changed = 0;
    if (PORTC.F0 == 0) {  // nut mode dc nhan
       Delay_ms(50);
       if (PORTC.F0 == 0) {
          mode++;
          changed = 1;  // danh dau co nut dc nhan
          if (mode > 2) mode = 0;
          while(PORTC.F0 == 0);
       }
    }
     return changed;

}

int up_down_button(){
    unsigned short changed = 0;
    char temp_str[7];

    if (portc.f1 == 0) {   // nut up dc nhan
        Delay_ms(50);
        if (portc.f1 == 0) {
           if (mode == 1 && temp_hot_lcd < 95){
                temp_hot_lcd += 5;
                changed = 1;  //danh dau co nut dc nhan
                Glcd_Box(30, 28, 70, 40, 0);   //cap nhat gia tri nhiet do moi len lcd
                IntToStr(temp_hot_lcd, temp_str); 
                Ltrim(temp_str); //xoa khoang trang o dau
                Glcd_Write_Text(temp_str, 55, 4, 1);
                Glcd_Write_Text("*C", 70, 4, 1);
           }
           if (mode == 2 && temp_cold_lcd < 9) {
                temp_cold_lcd ++;
                changed = 1;
                Glcd_Box(85, 28, 120, 40, 0);
                IntToStr(temp_cold_lcd, temp_str);
                Ltrim(temp_str);
                Glcd_Write_Text(temp_str, 85, 4, 1);
                Glcd_Write_Text("*C", 100, 4, 1);
           }
           while(portc.f1 == 0);
        }
    }

    if (portc.f2 == 0) {  //neu nut down dc nhan
        Delay_ms(50);
        if (portc.f2 == 0) {
           if (mode == 1 && temp_hot_lcd > 80) {
                temp_hot_lcd -= 5; //giam nhiet do
                changed = 1;
                Glcd_Box(30, 28, 70, 40, 0);
                IntToStr(temp_hot_lcd, temp_str);
                Ltrim(temp_str);
                Glcd_Write_Text(temp_str, 55, 4, 1);
                Glcd_Write_Text("*C", 70, 4, 1);
           }
           if (mode == 2 && temp_cold_lcd > 5) {
                temp_cold_lcd --;
                changed = 1;
                Glcd_Box(85, 28, 120, 40, 0);
                IntToStr(temp_cold_lcd, temp_str);
                Ltrim(temp_str);
                Glcd_Write_Text(temp_str, 85, 4, 1);
                Glcd_Write_Text("*C", 100, 4, 1);
           }
           while(portc.f2 == 0);
        }
    }
    return changed;
}

int donhietdobinh(unsigned char channel){
    unsigned int adc_value;
    adc_value = ADC_Read(channel);
    return ((adc_value * 5.0)/ 1023.0) * 100.0; //chuyen doi sang do C
}

void dieuchinhnhietdo(int temp_binh, unsigned char che_do){
     if (che_do == 1 && nuocnong == 1) {  // HOT
         if (temp_binh <= (temp_set_hot - 1)) porta.f1 = 1; // neu nhiet do binh < nhiet do setting -1 thi bat heater
         if (temp_binh >= (temp_set_hot + 1)) porta.f1 = 0; // neu nhiet do binh > nhiet do setting +1 thi tat heater
     }
     if  (che_do == 2 && nuoclanh == 1) {  // COLD
         if (temp_binh <= (temp_set_cold - 1)) porta.f2 = 0; // neu nhiet do binh < nhiet do setting -1 thi bat cooler/block
         if (temp_binh >= (temp_set_cold + 1)) porta.f2 = 1;// neu nhiet do binh > nhiet do setting +1 thi tat cooler/block
     }
}

void xuatnuoc(){
     if (mode == 0 && binhnuoc == 1) { // an 1 lan: xuat nuoc, an them lan nua: khoa nuoc
         porta.f5 = ~porta.f5; // xa nuoc neu r5 = 1 va khoa nuoc neu ra5 = 0
         Delay_ms(50);
     }
     if (mode == 1 && binhnuoc == 1) {
         porta.f3 = ~porta.f3;
         Delay_ms(50);
     }
     if (mode == 2 && binhnuoc == 1) {
         porta.f4 = ~porta.f4;
         Delay_ms(50);
     }
}

void main() {
    ANSEL = 0b11000000; 
    ANSELH = 0;
    ADC_Init();
    C1ON_bit = 0;
    C2ON_bit = 0;

    TRISB = 0b00110001;
    TRISA = 0;
    TRISD = 0;
    TRISE1_bit = 1;
    TRISE2_bit = 1;
    TRISE0_bit = 0;
    TRISC = 0b00001111;

    PORTD = 0;
    PORTB = 0;
    PORTE = 0;
    PORTA = 0;

    portc.f7 = 0;

    pump_running = 0; pump_timeout = 0;
    OPTION_REG = 0b00000111; //dien tro treo
    WPUB = 0b00000001; //cho phep dien tro treo RB0

    Delay_ms(100);

    Glcd_Init();
    Delay_ms(100);

    Glcd_Fill(0x00);
    Glcd_Set_Font(Font5x7, 5, 7, 32); //set font

    Glcd_Write_Text("Khoi dong...", 25, 2, 1);
    Glcd_Write_Text("Vui long cho", 20, 4, 1);
    Glcd_Write_Text("10 giay", 35, 6, 1);
    Delay_ms(50);

    binhnuoc = PORTB.F0;    //doc trang thai ban dau ssor5 (co nuoc hay ko)
    Delay_ms(200);

     // doc trang thai ban dau cua binh nong xem co nuoc hay k

     if (PORTB.F4 == 1) nuocnong = 1;
     else nuocnong = 0;

    //tuong tu nhg la binh cold

    if (PORTB.F5 == 1) nuoclanh = 1;
    else nuoclanh = 0;


    if (binhnuoc == 1) {
        INTEDG_bit = 0;
    } else {
        INTEDG_bit = 1;
    }

    INTF_bit = 0;
    INTE_bit = 1;

    RBIF_bit = 0;
    RBIE_bit = 1;
    IOCB = 0b00110000;

    TMR0 = 0;
    TMR0IE_bit = 1;

    flag_ngat = 0;
    flag_ngat2 = 0;
    flag_timer0 = 0;
    timer0_count = 0;
    pump_timer_count = 0;
    pump_running = 0;
    pump_timeout = 0;

    temp_hot_lcd = temp_hot_default; //luc dau dat lcd = default, setting = default
    temp_cold_lcd = temp_cold_default;
    temp_set_hot = temp_hot_default;
    temp_set_cold = temp_cold_default;

    Delay_ms(10000);
    Draw_Interface();
    Delay_ms(2000);

    GIE_bit = 1;
    PEIE_bit = 1;
    porta.f0 = 1; portb.f1 = 0; porte.f0 = 0; //mac dinh o che do NEU
    while (1) {
        if(button_mode()) {   //xet mode co dc nhan hay k
            if (mode == 0) {porta.f0 = 1; portb.f1 = 0; porte.f0 = 0;}
            if (mode == 1) {porta.f0 = 0; portb.f1 = 1; porte.f0 = 0;}
            if (mode == 2) {porta.f0 = 0; portb.f1 = 0; porte.f0 = 1;} }
        Delay_ms(50);

        if (up_down_button()) { //xet xem co nut up down hay k
            temp_set_hot = temp_hot_lcd; //dat gia tri nhiet do setting = gt tren lcd
            temp_set_cold = temp_cold_lcd;
        }
        //xuat nuoc khi nhan nut xuat
        if (portc.f3 == 0) {
            Delay_ms(50);
            if (portc.f3 == 0) {
                xuatnuoc();
                while (portc.f3 == 0);
            }
        }
        Delay_ms(50);

        if (flag_ngat == 1) {
            flag_ngat = 0;
            Delay_ms(50);

            // cap nhat hien thi
            if (pump_timeout && binhnuoc == 0) {
                // Timeout: hien thi 10s
                binhnuoc = 0;
            }
            mucnuoc_binh(); //ve lai binh nuoc dua tren trang thai hien tai
        }

        Delay_ms(50);

        // xli motor bom nuoc = timer0
        if (binhnuoc == 0) {  // Binh can (RB0=0)
            if (!pump_running && !pump_timeout) {
                // Bat dau bom nuoc
                portc.f7 = 1; //bat cr7 de bat motor ( 7 gio kem 10)

                pump_running = 1;
                pump_timer_count = 0;
            }

            // Kiem tra timeout
            if (pump_timeout) {
                // Da timeout 60s ma van chua day -> hien thi 10%
                portc.f7 = 0;  // Tat motor

                binhnuoc = 0;  // Dam bao hien thi 10%
                mucnuoc_binh();
            }
        } else {
            // BBinh day (RB0=1)
            portc.f7 = 0;

            pump_running = 0;
            pump_timer_count = 0;
            pump_timeout = 0;  // Reset timeout khi day

            if (flag_ngat2) {
                flag_ngat2 = 0;
                Delay_ms(50);
            }

            if (flag_timer0) {
                flag_timer0 = 0; //moi 5s doc nhiet do binh va dieu chinh nhiet do 1 lan
                Delay_ms(50);
                if (nuocnong) {
                    nhietdobinhnong = donhietdobinh(6); //doc nhiet do binh nong
                    dieuchinhnhietdo(nhietdobinhnong, 1); //dieu chinh nhiet do binh nong
                }
                Delay_ms(200);
                if (nuoclanh) {
                    nhietdobinhlanh = donhietdobinh(7);
                    dieuchinhnhietdo(nhietdobinhlanh, 2);
                }
            }
        }
        Delay_ms(50);

    }
}