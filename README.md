# ✅ Lista de pruebas de la ROM CM7 – LG L3 E400

## 🔧 Sistema base

| Prueba                          | Resultado | Comentario                          |
|--------------------------------|-----------|-------------------------------------|
| Arranque del sistema           | ✅        | Inicia correctamente sin errores    |
| Rotación automática de pantalla| ✅        | Gira con el sensor, sin rayas (parche de stride en SurfaceFlinger) |
| Apagado y reinicio             | ✅        | Funciona normalmente                |
| Modo avión                     | 🟡        | No probado aún                      |
| Estabilidad general            | ✅        | No hay reinicios aleatorios         |
| Tiempo de arranque (<1min)     | ✅        | Boot en ~20 segundos                |
| Instalación desde recovery     | ✅        | Instalación sin errores             |

## 📱 Pantalla y gráficos

| Prueba                        | Resultado | Comentario                             |
|------------------------------|-----------|----------------------------------------|
| Resolución correcta          | ✅        | 240x320 sin distorsión                 |
| Brillo ajustable             | ✅        | Funciona con barra de ajustes          |
| Aceleración gráfica HW       | ❌        | Gráficos se ven lentos y con glitches  |
| Táctil funcional             | ✅        | Detecta toques y multitouch            |
| Renderizado de íconos/texto  | ✅        | Sin artefactos visuales                |

## 🔊 Audio

| Prueba                  | Resultado | Comentario                       |
|------------------------|-----------|----------------------------------|
| Reproducción de audio  | ✅        | MP3 y tonos del sistema OK       |
| Volumen ajustable      | ✅        | Se ajusta desde botones físicos  |
| Altavoz externo        | ✅        | Audio fuerte y claro             |
| Auriculares            | 🟡        | No probado                       |
| Grabadora de voz       | ❌        | Funcionando                      |
| Micrófono              | ✅        | Funciona en llamadas             |

## 📶 Conectividad

| Prueba                 | Resultado | Comentario                            |
|-----------------------|-----------|---------------------------------------|
| Wi-Fi                 | ✅        | Se conecta y navega sin problemas     |
| Bluetooth             | ✅        | Verificado a mano (kernel del stock)  |
| Datos móviles         | ✅        | Detecta señal y navega                |
| APN detectado         | ✅        | Automático con chip Claro             |
| GPS                   | 🟡        | Detecta satélites pero no fija aún    |

## 📷 Cámara

| Prueba                       | Resultado | Comentario                        |
|-----------------------------|-----------|-----------------------------------|
| Cámara trasera (foto/video) | ✅        | Foto 2048x1536 y video MPEG-4 640x480 (encoder HW) |
| Enfoque                     | 🟡        | El HAL no reporta autofocus (foco fijo) |
| App de cámara               | ✅        | Abre y funciona                   |

## 📂 Almacenamiento

| Prueba                       | Resultado | Comentario                                 |
|-----------------------------|-----------|--------------------------------------------|
| Almacenamiento interno      | ✅        | Se puede explorar con file manager         |
| SD externa                  | ✅        | Monta mmcblk1p1 correctamente              |
| Montaje automático de SD    | ✅        | Detecta al insertar                        |
| Almacenamiento por USB      | ❌        | No aparece en PC                           |
| Escritura/lectura en /data  | ✅        | Pruebas OK                                 |

## 🔋 Batería y energía

| Prueba                          | Resultado | Comentario                       |
|--------------------------------|-----------|----------------------------------|
| Carga por USB                   | ✅        | Icono de carga se muestra        |
| Carga con el teléfono apagado   | ✅        | Animación del stock (`chargerlogo` V10c) |
| Nivel de batería correcto       | ✅        | Cambia de forma realista         |
| Suspensión y despertar          | 🟡        | Se apaga pantalla pero no despierta siempre |

## 🎮 Sensores y botones

| Prueba                     | Resultado | Comentario                         |
|---------------------------|-----------|------------------------------------|
| Sensor de proximidad      | 🟡        | No probado                         |
| Acelerómetro              | ❌        | SensorService no puede iniciar     |
| Botón Power               | ✅        | Funciona                           |
| Botones volumen           | ✅        | Funcionan                          |
| Botón home                | ✅        | Funciona                           |
| Botones capacitivos       | ✅        | Back y menú funcionales            |

## 🧩 Aplicaciones básicas

| Prueba               | Resultado | Comentario                        |
|---------------------|-----------|-----------------------------------|
| Llamadas             | ✅        | Se puede marcar y recibir         |
| SMS                  | ✅        | Envío y recepción OK              |
| Contactos            | ✅        | Funciona                          |
| Navegador web        | ✅        | Abre Google, funciona             |
| Calendario           | ✅        | Se puede agregar eventos          |
| Play Store (GApps)   | ❌        | No instaladas en esta versión     |
| Teclado              | ✅        | Responde bien                     |

## 📲 Rendimiento

| Prueba                        | Resultado | Comentario                         |
|------------------------------|-----------|------------------------------------|
| Multitarea básica             | ✅        | Dos apps sin lag                   |
| Fluidez del launcher          | ✅        | Sin tirones                        |
| Cuelgues en apps comunes      | ❌        | Algunas apps crashean (Camera)     |
| Logcat sin errores críticos   | 🟡        | Algunos Warnings por sensores      |

## 📜 Extras

| Prueba                        | Resultado | Comentario                         |
|------------------------------|-----------|------------------------------------|
| Recovery funcional            | ✅        | TWRP inicia correctamente          |
| ADB                           | ✅        | `adb shell` funciona               |
| Root                         | ✅        | `su` da root sin errores           |
| Montaje de particiones        | ✅        | /system, /data y /cache montados   |
> Notas de bring-up, causas raíz y problemas abiertos: [docs/E400_NOTES.md](docs/E400_NOTES.md)
