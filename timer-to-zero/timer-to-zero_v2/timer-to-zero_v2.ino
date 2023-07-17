#define CLK 2
  #define DIO 3

  #include <GyverTM1637.h>
  GyverTM1637 disp(CLK, DIO);

  enum state  {Setup1, ZeroTime, Kaboom, Idle};
  int current_state = Setup1;
  int time_on  = 3600;
  int time_off = 1;
  uint32_t tic = 0;
  uint32_t tac = 0;
  uint32_t boom = 9; //время запала
  uint32_t myTimer = 0;
  uint32_t timerStop = 0;



  void setup() {
    Serial.begin(9600);
    disp.clear();        // чистим дисплей
    disp.brightness(7);  // яркость, 0 - 7 (минимум - максимум)
    pinMode(4, INPUT_PULLUP); // старт
    pinMode(5, INPUT_PULLUP); // стоп
    pinMode(6, INPUT_PULLUP); // верх
    pinMode(7, INPUT_PULLUP); // вниз
    pinMode(8, INPUT_PULLUP); // триггер
    analogWrite(A0, 255); // реле
  }

  void loop() {
    // Set Value
    bool buttonPlus  = false;
    bool buttonMinus = false;
    bool buttonStart = false;
    bool buttonStop  = false;
    bool timeEvent   = false;
    bool pushButton  = false;
    bool trigger     = false;

    bool DEBUG = false;  // Переменная для вывода специальной иформации в serialport

    if (digitalRead(4) == 0 ||
        digitalRead(5) == 0 ||
        digitalRead(6) == 0 ||
        digitalRead(7) == 0 ||
        digitalRead(8) == 0
       ) {
      delay(200);  //считывает состояние каждой кнопки каждые 200мс
                   //создает событие при нажатии на кнопку
      if (digitalRead(6) == 0) buttonPlus  = true, pushButton = true;
      if (digitalRead(7) == 0) buttonMinus = true, pushButton = true;
      if (digitalRead(4) == 0) buttonStart = true, pushButton = true;
      if (digitalRead(5) == 0) buttonStop  = true, pushButton = true;
      if (digitalRead(8) == 0) trigger     = true, pushButton = true;
      
     
    }
   if (DEBUG) {
        String dbgStr = String("Status button | ")
                      + String(buttonPlus)  
                      + String(buttonMinus)        
                      + String(buttonStart) 
                      + String(buttonStop)                              
                      + String(trigger);                                                 
        Serial.println(dbgStr);  // Вывод на экран состояния нажатия кнопок
      }  
    // Запуск посекундного таймера, для точного отсчета времени
    if (millis() - myTimer >= 1000) {
      myTimer = millis();
      timeEvent = true;
    }
    
    //////////////////////// Состояние настройки ////////////////////////////
    if (current_state == Setup1) { // кнопками задаем время
      if      (buttonPlus)  time_on = time_on + 10;
      else if (buttonMinus) time_on = time_on - 10;
      else if (buttonStart) current_state = Idle, tic = 0;
      
      if      (time_on < 1) time_on = 1;  // проверка на дурака, что бы нельзя было задать время с отрицательным значением
            if (DEBUG) {
          String dbgStr = String("Current state = ")
                        + String(current_state) 
                        + String(" | time on  = ")
                        + String(time_on)  
                        + String(" | time_off = ") 
                        + String(tic );
          Serial.println(dbgStr);  // Вывод на экран состояния 
        }  
      if      (timeEvent || pushButton) {
        disp.displayInt(time_on);

      }
    }
    //////////////////////// Состояние активации ////////////////////////////
    if (current_state == Kaboom){
      if (boom > tac){
        if(timeEvent){
        disp.displayInt(8888);
        analogWrite(A0, 0); // Инвертированное реле
        tac++;
        }
      } else {
        disp.displayInt(0000);
        analogWrite(A0,255); // Выключаем реле
      }
      
      if (DEBUG) {
          String dbgStr = String("Current state = ")
                        + String(current_state) 
                        + String(" | boom     = ") 
                        + String(boom) 
                        + String(" | tac      = ") 
                        + String(tac );
          Serial.println(dbgStr);  // Вывод на экран состояния 
      }
    }

    //////////////////////// Состояние обратного отсчета ////////////////////////////
    if (current_state == ZeroTime) {
      if (trigger == false){
        current_state = Idle;
      }
      
      if (time_on > tic){   
        if (timeEvent) {
          disp.displayInt(time_on - tic), Serial.println(time_on - tic); // добавить минус на дисплей для понимания состояния
          tic++;
          }
      } else {
        current_state = Kaboom;
      }
      
      if (DEBUG) {
          String dbgStr = String("Current state = " )
                        + String(current_state) 
                        + String(" | time on  = ")
                        + String(time_on) 
                        + String(" | time_off = ")
                        + String(tic );
          Serial.println(dbgStr);  // Вывод на экран состояния 
        }
    }

    //////////////////////// Состояние ожидания ////////////////////////////
    if (current_state == Idle) {
      disp.displayInt(time_on);
      if( trigger && timeEvent){
        current_state = ZeroTime;
        tic = 0;
      }  
      if (DEBUG) {
          String dbgStr = String("Current state = ") 
                        + String(current_state) 
                        + String(" | time on  = ")
                        + String(time_on)  
                        + String(" | time_off = ")
                        + String(tic );
          Serial.println(dbgStr);  // Вывод на экран состояния 
        }
    }
    
    
  
    

  }
