/*Possíveis erros do arduino:
  Stack Overflow: Ocorre quando se excede a capacidade da pilha de uma tarefa.
  Neste caso o led pisca lentamente. Verifique o tamanho das pilhas das tarefas
  e aumente a que demandar mais memória.

  Falha de Heap: Ocorre quando não se consegue alocar memória dinamicamente.
  Neste caso o led pisca rapidamente. Verifique as tarefas,
  tente reduzir as pilhas das tarefas mais simples
  (não exigem muita memória) ou diminuir o número de tarefas.
*/

//Inclusão das bibliotecas
#include "Arduino_FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include <LiquidCrystal.h>
#include <math.h>

//Porta serial
#define SEND_GANTT  1

//LCD
const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//Button
const int pinoBotaoVerde = 12;

//Tasks
TaskHandle_t        timerTaskH;
TaskHandle_t        cronometroTaskH;
TaskHandle_t        saudacaoAutomaticaTaskH;
TaskHandle_t        saudacaoManualTaskH;

//Mutex - capacidade da serial - mutex = uma tarefa por vez
SemaphoreHandle_t   SerialMutex;

//Elementos para o temporizador
boolean desativarTemporizador = false;
int minutoTemporizador = 1, segundoTotalTemporizador = 59;
int segundoTemporizador, ultimoTempoTemporizador;
unsigned long salvarTempoTemporizador = millis(), proximoSegundoTemporizador;

//Elementos para cronometro
int segundosCronometro;
unsigned long millisBaseCronometro = millis();
int minutosCronometro = 0;

//Exibicao das informacoes no monitor serial e administracao da utilizacao da porta serial
void sendGantt(const char *name, unsigned int stime, unsigned int etime) {
  if (xSemaphoreTake(SerialMutex, portMAX_DELAY) == pdTRUE) { //Solicita Mutex
    Serial.print("\t\t");
    Serial.print(name);
    Serial.print(": ");
    Serial.print(stime);
    Serial.print(", ");
    Serial.println(etime);
    xSemaphoreGive(SerialMutex);                            //Libera Mutex
  }
}

void setup() {
  //Inicializa LCD
  lcd.begin(16, 2);

  //Configura o botao
  pinMode(pinoBotaoVerde, INPUT_PULLUP);

  //Inicializa Serial
  Serial.begin(9600);
  Serial.print("1s is ");
  Serial.print(configTICK_RATE_HZ);
  Serial.print(" ticks at ");
  Serial.print(F_CPU);
  Serial.print(" Hz\n\n");
#if (defined(SEND_GANTT) && (SEND_GANTT==1))
  Serial.println("gantt\n\tdateFormat x\n\ttitle A gant diagram");
#endif

  SerialMutex = xSemaphoreCreateMutex();

  //Cria tarefa timerTask
  xTaskCreate(timerTask,            //Funcao
              "timerTask",          //Nome
              180,                //Pilha
              NULL,               //Parametro
              1,                  //Prioridade
              &timerTaskH);

  //Cria tarefa cronometroTask
  xTaskCreate(cronometroTask,
              "cronometroTask",
              180,
              NULL,
              1,
              &cronometroTaskH);

  //Cria tarefa saudacaoAutomaticaTask
  xTaskCreate(saudacaoAutomaticaTask,
              "saudacaoAutomaticaTask",
              128,
              NULL,
              2,
              &saudacaoAutomaticaTaskH);

  //Cria tarefa saudacaoManualTask
  xTaskCreate(saudacaoManualTask,
              "saudacaoManualTask",
              128,
              NULL,
              2,
              &saudacaoManualTaskH);
}

void loop() {
  // Nada é feito aqui, Todas as funções são feitas em Tasks
}

//Timer
void timerTask(void *arg) {
  unsigned int stime;

  while (1) {
    stime = millis();
    lcd.setCursor(0, 0);
    Temporizador(desativarTemporizador); //Chama a funcao do temporizador
    vTaskDelay(pdMS_TO_TICKS(500));                //Espera 0.5s
#if (defined(SEND_GANTT) && (SEND_GANTT==1))
    sendGantt("Timer", stime, millis());         //Envia Informações pela Serial
#endif
  }
  //O codigo nunca deve chegar aqui
  vTaskDelete(NULL);      //Deleta a Task atual
}

//Cronometro
void cronometroTask(void *arg) {
  unsigned int stime;

  while (1) {
    stime = millis();
    Cronometro(); //Chama o a funcao cronometro
    vTaskDelay(pdMS_TO_TICKS(500));                //Espera 0.5s
#if (defined(SEND_GANTT) && (SEND_GANTT==1))
    sendGantt("Cronometro", stime, millis());         //Envia Informações pela Serial
#endif
  }
  //O codigo nunca deve chegar aqui
  vTaskDelete(NULL);      //Deleta a Task atual
}

//Saudacao automatica
void saudacaoAutomaticaTask(void *arg) {
  unsigned int stime;

  while (1) {
    stime = millis();
    lcd.setCursor(10, 0);
    lcd.print("Proj.");
    vTaskDelay(pdMS_TO_TICKS(1000));                //Espera 0.5s
    lcd.setCursor(10, 0);
    lcd.print("-AP2-");
    vTaskDelay(pdMS_TO_TICKS(1000));
#if (defined(SEND_GANTT) && (SEND_GANTT==1))
    sendGantt("Saudacao Automatica", stime, millis());         //Envia Informações pela Serial
#endif
  }
  //O codigo nunca deve chegar aqui
  vTaskDelete(NULL);      //Deleta a Task atual
}

//Saudacao manual
void saudacaoManualTask(void *arg) {
  unsigned int stime;

  while (1) {
    stime = millis();
    if (digitalRead(pinoBotaoVerde) == LOW) {
      lcd.setCursor(10, 1);
      lcd.print("On.");
    }
    vTaskDelay(pdMS_TO_TICKS(100));
    if (digitalRead(pinoBotaoVerde) == LOW) {
      lcd.setCursor(10, 1);
      lcd.print("Off");
    }
    vTaskDelay(pdMS_TO_TICKS(100));
#if (defined(SEND_GANTT) && (SEND_GANTT==1))
    sendGantt("Saudacao Manual", stime, millis());
#endif
  }
  //O codigo nunca deve chegar aqui
  vTaskDelete(NULL);      //Deleta a Task atual
}
