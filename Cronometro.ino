boolean Cronometro () {

  segundosCronometro = ((millis() - millisBaseCronometro) / 1000);

  lcd.setCursor(0, 1);
  lcd.print("-");
  lcd.print(minutosCronometro);
  lcd.print(":");
  lcd.print(segundosCronometro);
  lcd.print("-");

  if (segundosCronometro > 59) {

    ++minutosCronometro;
    millisBaseCronometro = millis();

  }
}
