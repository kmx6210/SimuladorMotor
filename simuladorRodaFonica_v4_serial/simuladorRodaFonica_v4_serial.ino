////                                      Simulador de sinal Roda Fonica sem sinal de fase
//                                        ver 3.0   04-07-2022
// todo
// recebimento dos comandos pela serial 
// ligar - liga o motor 
// desligar - desliga o motor
// acelera=xxx - acelera para xx onde xx varia de 0 a 100
//

// Controle de temp. 04-07-2022
//   * OKtd01-Possivel colocar o temporizador Temperadura junto com RPM
//   * OKtd02-Falta ligar o pini digital da Ventoinha
//   * td03-Falta escrever o valor da Temperatura em a1
// Controle de Aceleração 04-07-2022
//   * td04-receber o sinal da injeção.
//   * td05-Comparar com um mapa de Injeção x rotação
//   * td06-montar o PI para pegar o erro entre a injecao e a rotação, aplicar o erro na aceleração.
//   * td07-Ajustar os valores de :
//     td071- Sense, que irá atualizar a rotação
//     td072- ajustar os valores da Sonda Lambda contra o mapa
//     td073- 
//  Posicao da Borboleta ...
//   * td08-Usar seta a esquerda e a direita para desacelerar e acelerar
//  Temperatura do ar
//     
///////////////////////////////////////////////////////////////////////////////////

////                                       Usei o modelo DPF ELETRÔNICA
////                                    SIMULADOR DE SINAL DE RODA FONIA
////                                    VER 2.1   18/08/2020
///////////////////////////////////////////////////////////////////////////////////
//; pinos disponiveis

// 0
// 1
// 2
// 3
// 4 
// 5 
// 6 
// 7 
// 8 
// 9 
//10 X Entrada Ventoinha
//11 X Vela (Apenas uma)
//12 X Injecao (Apenas uma)
//13 x Saida Roda Fonica

// Analogicas
// A0 - Leitura teclado
// A1 - Posicao Borboleta
// A2 - Regime de Carga (presssao Coletor)
// A3 - Temp Motor
// A4 - Temp Ar
// A5 - Sonda lambda




const int pinoVentoinha = 10;

///Variaveis de memória
int a=0,b=0, dente=0,  rpm=0, sensor=10,  ciclo=0, cont=0, rotacao=0, fonica=0, contfase=0;
//int analogPin = A1;
int ndentes=60,nfalha=2, ON=0;
int menu=0, lastMenu = -1, fase1i=0 ,fase1f=0, fase2i=0,fase2f=0,fase3i=0, fase3f=0, fase4i=0, fase4f=0,fase5i=0, fase5f=0, teclacont=0;
int opc = 0, lastOpc = -1;
int ligado=0, lligado=0, acel = 0, lacel = 0, mostraStatus=0;
// Controle de Temperatura
int temp=0, tempm = 20, ltempm=20;
boolean b_ventoinha=false;
String comando="";
int tempEsquenta = 22530;
int tempEsfria = 26500;

//carrega a biblioteca eeprom
#include <EEPROM.h> 

////////////////////////
void setup(void)
{//////carrega valores da eeprom
  fase1i =EEPROM.read(0);
  fase1f =EEPROM.read(1);
  fase2i =EEPROM.read(2);
  fase2f =EEPROM.read(3);
  fase3i =EEPROM.read(4);
  fase3f =EEPROM.read(5);
  fase4i =EEPROM.read(6);
  fase4f =EEPROM.read(7);
  fase5i =EEPROM.read(10);
  fase5f =EEPROM.read(11);
  nfalha =EEPROM.read(8);
  ndentes =EEPROM.read(9);
  tempEsquenta = EEPROM.read(12)+256*EEPROM.read(13);
  tempEsfria = EEPROM.read(14)+256*EEPROM.read(15);

////////////////////////////////////
//define valores na primeira inicialização
  if(ndentes==255){
    ndentes=60;
    nfalha=2;
    fase1i=0;
    fase1f=0;
    fase2i=0;
    fase2f=0;
    fase3i=0;
    fase3f=0;
    fase4i=0;
    fase4f=0; 
    fase5i=0;
    fase5f=0;
    tempEsquenta = 22530;
    tempEsfria = 26500;
    
  }
///////////////////////////////
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
 // inicializa timer1 
  noInterrupts();           // disabilita todas as interrupções
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  //OCR1A = 31250;            // compare match register 16MHz/256/2Hz
  OCR1A = 1;
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
  interrupts();             // enable all interrupts

  // estabelece a comunicão
  Serial.begin(115200);
  while (! Serial){
    ;// espera a Serial Comunicar
  }

}

ISR(TIMER1_COMPA_vect)          // timer compare interrupt service routine
{
   cont++;
   fonica++;
   if(cont>=31250){
     rotacao=rpm;
     rpm=0;
     cont=0;
   }
   ciclo++;
   if(ciclo>22){
     ciclo=0;
   }
   if(ON==1){/// liga sinal
     if(fonica>sensor){
//   * td01-Possivel colocar o temporizador Temperadura junto com RPM
       temp++;
       fonica=0;
       if(nfalha>0){ 
         if(dente>ndentes-nfalha){  //falha
           a=1;
         }
       }
       if(a==0){
         digitalWrite(13, LOW);   
         a=1;
       }
       else{                     
         digitalWrite(13, HIGH);
         a=0;
  
         if(dente==ndentes){ ////Quatidade dentes da roda
           dente=1;
         }
         if(contfase+1==ndentes*2){
           contfase=1;
           dente=1;//////  
         }
       }
       if(b==0){
         b=1;
         rpm++;
         dente++;
         contfase++;
       }else{
         b=0;
  
       }
     } 
   
  //////////////////////sinal de fase
  }
}
void tempMotor() {

//{   * td02-Falta ligar o pini digital da Ventoinha
  int n_ventoinha;
  n_ventoinha = digitalRead(pinoVentoinha);
  if (n_ventoinha) {
    b_ventoinha = true; 
  } else {
    b_ventoinha = false;
  }
//}   * td02-Falta ligar o pini digital da Ventoinha

  if (b_ventoinha) {
    if (temp > tempEsfria) {
      tempm --;
      temp=0;
    }
  } else {
    if (ON){
      if (temp > tempEsquenta) {
        tempm++;
        temp = 0;
      }
    }
  }

}
void analisaCmd(String cmd){
  String resultado = "";
  String sz_cmd="", sz_parametro="";
  int parametro=0;
  //
  // localiza o cmd, o operando se existir e retorna a acao
  // procura por iqual
  int iqual = cmd.indexOf("=");
  if (iqual != -1) { // Existe iqual, aceita o parametro ate o fim da linha
    sz_cmd = cmd.substring(0, iqual);
    sz_parametro = cmd.substring(iqual+1);
    parametro = sz_parametro.toInt();
    resultado = "Cmd : " + sz_cmd;
    resultado = resultado + "\nParametro : " + sz_parametro;
  } else {
    sz_cmd = cmd;
    resultado = "Cmd : "+sz_cmd;
  }
// ligar - liga o motor 
// desligar - desliga o motor
// acelera=xxx - acelera para xx onde xx varia de 0 a 100

  if (sz_cmd.equals("acel")) {
    acel = parametro;
  }
  if (sz_cmd.equals("esquenta")) {
    tempEsquenta = parametro;
  }
  if (sz_cmd.equals("esfria")) {
    tempEsfria = parametro;
  }
  
  if (sz_cmd.equals("ligar")) {
    ligado=1;
  }
  if (sz_cmd.equals("desligar")) {
    ligado = 0;
  }
  mostraStatus = 0;
  if (sz_cmd.equals("status")) {
    mostraStatus = 1;
  }
  if (sz_cmd.equals("salva")) {
    salvaEprom();
    mostraStatus = 1;
  }
 
}

void loop(void)
{
  //sensor = analogRead(A1)/10;
  int incomingByte = 0;
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();
    if (incomingByte != 13){
      comando += String(char(incomingByte));
    } else {
      analisaCmd(comando);
      //Serial.println(analisaCmd(comando));
      comando="";
      if (ligado != lligado) {
        lligado = ligado;
        if (ligado){
          ON = 1;
        } else {
          ON = 0;
        }
        saidaLcd();
      }
      if (acel != lacel) {
        lacel=acel;
        sensor = (100-acel)/10;        
        saidaLcd();
        
      }
      if (mostraStatus) {
        saidaLcd();
      }
    }
  }

  tempMotor();
//  if (opc != lastOpc) {
//      menuPrincipal(opc);
//      lastOpc = opc;
//  }
//  if (cont == 0){
//    saidaLcd();
//  }
  if (ltempm != tempm) {
    ltempm = tempm;
    saidaLcd();
  }
  
}

void saidaLcd() {
  Serial.print("Roda Fonica");
  Serial.print(ndentes);
  Serial.print("-");
  Serial.println(nfalha);
  
  if (ON) {
    Serial.println("Motor Ligado ");
  } else {
    Serial.println("Motor Desligado ");
  }
  Serial.print(": Sensor : ");
  Serial.print(sensor);
  Serial.print(" : Rotação : ");
  Serial.print(rotacao);
  Serial.print(" : opc : ");
  Serial.println(opc);

  Serial.print("Temp. Motor : ");
  Serial.print(tempm);
  if (b_ventoinha) {
    Serial.println(" Ventoinha Ligada");
  } else {
    Serial.println(" Ventoinha Desligada");
  }
  if (mostraStatus) {
    Serial.println("Parametros : ");
    Serial.print("TempEsquenta : ");
    Serial.println(tempEsquenta);
    Serial.print("TempEsfria : ");
    Serial.println(tempEsfria);
    
  }
  
}
void salvaEprom() {
  ////grava valores na eeprom
EEPROM.update(0,fase1i );
EEPROM.update(1,fase1f );
EEPROM.update(2,fase2i );
EEPROM.update(3,fase2f );
EEPROM.update(4,fase3i );
EEPROM.update(5,fase3f );
EEPROM.update(6,fase4i );
EEPROM.update(7,fase4f );
EEPROM.update(8,nfalha );
EEPROM.update(9,ndentes );
EEPROM.update(10,fase5i );
EEPROM.update(11,fase5f );
byte maisalto = tempEsquenta/256;
byte baixo = tempEsquenta % 256;
EEPROM.update(12,baixo );
EEPROM.update(13,maisalto);
maisalto = tempEsfria/256;
baixo = tempEsfria % 256;
EEPROM.update(14,baixo );
EEPROM.update(15,maisalto );
int tempEsquenta = 22530;
int tempEsfria = 26500;

}
void menuPrincipal(int opc) {
  if ((opc > 600) and
     (opc < 800)) { // enter
     if (ON == 0){
      ON = 1; 
     } else {
      ON = 0;
     }
     saidaLcd(); 
  }
  if ((opc > 60) and
     (opc < 200)) { // up
      sensor += 1;
     saidaLcd(); 
  }
  if ((opc > 200) and
     (opc < 400)) { // down
      sensor -= 1;
     saidaLcd(); 
  }
  
}
//////////////
