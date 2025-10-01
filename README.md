# SRI Performance
Sistema de Riego Inteligente sectorizado y autónomo.

## Descripción
SRI Performance es un sistema de riego inteligente que utiliza un ESP32, sensores de humedad y un aspersor direccionable en dos ejes (X–Y) para regar únicamente donde es necesario. Optimiza el uso del agua y mantiene el césped saludable sin intervención constante.

## Características
- Riego automático basado en humedad real del suelo.
- Aspersor 2D (X–Y) para direccionar el riego por sectores.
- Control desde pantalla local o mediante Wi-Fi/Bluetooth.
- Ahorro de agua superior al 20–40% en comparación con métodos tradicionales.
- Instalación prolija con cañerías y cableado subterráneo.
- Seguridad: evita encharcado, bloqueos y riego en horarios inadecuados.

## Tecnologías utilizadas
**Hardware:** ESP32, servomotores, bomba de agua, aspersor direccionable, display.  
**Sensores:** humedad de suelo, temperatura ambiente.  
**Software:** Arduino IDE / PlatformIO.  
**Comunicación:** Wi-Fi y Bluetooth.

## Funcionamiento
1. Los sensores miden humedad y temperatura.  
2. El ESP32 evalúa si es necesario regar.  
3. El aspersor se mueve hacia el sector requerido.  
4. La bomba activa el riego únicamente en esa zona.  
5. El sistema se detiene al alcanzar la humedad adecuada.

## Impacto esperado
- Ahorro significativo de agua en jardines y espacios residenciales.  
- Reducción de costos de mantenimiento.  
- Uso más responsable de los recursos hídricos.
