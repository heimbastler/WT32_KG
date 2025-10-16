




// ===================================================
// Touch Panel Belegung (3x MPR121)
// ===================================================

 /*
 TouchBoard 2: Säule Garten EG (I2C 0x5C)
 TouchBoard 3: Säule Strasse EG (I2C 0x5D)

 #############################################
 ##                     ##                  ##
 ##    toggleLamps      ##    toggleLamps   ##
 ##       case 9        ##      case 11     ##
 ##                     ##                  ##
 #############################################
 ##                     ##                  ##
 ##     Steinlampe      ##   Kronleuchter   ##
 ##       case 6        ##      case 10     ##
 ##                     ##                  ##
 #############################################
 ##                     ##                  ##
 ##    Kuechenlampe     ##    EGFlurlampe   ##
 ##       case 2        ##      case 0      ##
 ##                     ##                  ##
 #############################################
 ##                     ##                  ##
 ## Kuechenarbeitslampe ##   Traegerlampen  ##
 ##       case 3        ##      case 1      ##
 ##                     ##                  ##
 #############################################




TouchBoard 1: Tür Garten EG (I2C 0x5A)
 
 ##############################################
 ##                     ##                   ##
 ##    ??               ##    ??             ##
 ##       case          ##      case         ##
 ##                     ##                   ##
 ##############################################
 ##                     ##                   ##
 ##    ??               ##    ??             ##
 ##       case          ##      case         ##
 ##                     ##                   ##
 ##############################################
 ##                     ##                   ##
 ##    ??               ##    ??             ##
 ##       case          ##      case         ##
 ##                     ##                   ##
 ##############################################
 ##                     ##                   ##
 ##    ??               ##    ??             ##
 ##       case          ##      case         ##
 ##                     ##                   ##
 ##############################################
 

case 2: toggleWohnzimmerlampe1(); break;    // oben 1te von links
case 5: toggleTraegerlampen(); break;       // oben 2te von links
case 1: toggleEGFlurlampe(); break;         // unten 1te von links
case 0: toggleAussenlampeGarten(); break;   // unten 2te von links
case 9: toggleFensterrolloUp(); break;      // Taster für Fensterrollo up
case 10: toggleFensterrolloDown(); break;   // Taster für Fensterrollo down
case 6: toggleTuerrolloUp(); break;         // push button for Türrollo up
case 11: toggleTuerrolloDown(); break;      // push button for Türrollo down


*/