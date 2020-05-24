boolean Temporizador (boolean desativado) {

  proximoSegundoTemporizador = salvarTempoTemporizador + 1000;

  segundoTemporizador = segundoTotalTemporizador - ultimoTempoTemporizador;
  lcd.setCursor(0, 0);
  lcd.print("-");
  lcd.print(minutoTemporizador);
  lcd.print(":");
  lcd.print(segundoTemporizador);
  lcd.print("-");

  if (segundoTemporizador <= 0) {
    //   minutoTemporizador = minutoTemporizador - 1;

    if (minutoTemporizador == 0 && segundoTemporizador <= 0) {
      //  lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Acabou!");
      return true;
    }
    
    minutoTemporizador = minutoTemporizador - 1;
    segundoTotalTemporizador = 60;
    ultimoTempoTemporizador = 0;

  }

  if (millis() >= proximoSegundoTemporizador) {
    ++ultimoTempoTemporizador;
    salvarTempoTemporizador = proximoSegundoTemporizador;
  }
}
