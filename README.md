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
| Aceleración gráfica HW       | ✅        | Adreno 200 en uso (kgsl-3d0 0666 por init) y `debug.composition.type=mdp` |
| Táctil funcional             | ✅        | Detecta toques y multitouch            |
| Renderizado de íconos/texto  | ✅        | Sin artefactos visuales                |

## 🔊 Audio

| Prueba                  | Resultado | Comentario                       |
|------------------------|-----------|----------------------------------|
| Reproducción de audio  | ✅        | MP3 y tonos del sistema OK       |
| Volumen ajustable      | ✅        | Se ajusta desde botones físicos  |
| Altavoz externo        | ✅        | Audio fuerte y claro             |
| Auriculares            | ✅        | Música por el jack; se detectan también si ya están conectados al arrancar |
| Grabadora de voz       | ✅        | Verificado por el usuario        |
| Micrófono              | ✅        | Funciona en llamadas             |

## 📶 Conectividad

| Prueba                 | Resultado | Comentario                            |
|-----------------------|-----------|---------------------------------------|
| Wi-Fi                 | ✅        | Se conecta y navega sin problemas     |
| Bluetooth             | ✅        | Verificado a mano (kernel del stock)  |
| Datos móviles         | 🟡        | Sin validar: ver fila de SIM          |
| APN detectado         | 🟡        | Sin validar (necesita registro en red)|
| SIM                   | 🟡        | SIM Movistar: READY/LOADED, registro denegado (¿SIM suspendida?); SIM de otro operador: bloqueo de red del equipo (e0_mov_la) |
| Radio FM              | ✅        | Sintoniza, suena por los audifonos con el volumen 0-15 como el stock y se apaga/enciende sin colgarse (requiere el kernel parcheado, `tools/patch_kernel_fm.py`) |
| GPS                   | 🟡        | Detecta satélites pero no fija aún    |

## 📷 Cámara

| Prueba                       | Resultado | Comentario                        |
|-----------------------------|-----------|-----------------------------------|
| Cámara trasera (foto/video) | ✅        | Foto 2048x1536 y video MPEG-4 640x480 (encoder HW) |
| App de cámara               | ✅        | Abre y funciona                   |

## 📂 Almacenamiento

| Prueba                       | Resultado | Comentario                                 |
|-----------------------------|-----------|--------------------------------------------|
| Almacenamiento interno      | ✅        | Se puede explorar con file manager         |
| SD externa                  | ✅        | Monta mmcblk1p1 correctamente              |
| Montaje automático de SD    | ✅        | Detecta al insertar                        |
| Almacenamiento por USB      | ✅        | Verificado por el usuario                  |
| USB OTG (host)              | ✅        | Pendrive vfat en /mnt/usbdisk; ver sección |
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
| Acelerómetro              | ✅        | BMA250, rotación probada           |
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
| Logcat sin errores críticos   | 🟡        | Warnings de Adreno (eglCreateImageKHR) |

## 🔌 USB OTG (host)

Probado el 2026-11-01 con un pendrive de 16 GB (vfat). El kernel stock 2.6.38 ya trae el host
(`msm_hsusb_host.0`, `usb-storage`, control en debugfs); lo que faltaba era el montaje y un interruptor.

- El pin ID del micro-USB no llega al SoC, así que no hay cambio automático: el modo host se fuerza
  por debugfs. `setprop sys.otg.mode host` (o la app `OtgSwitch`, `packages/apps/OtgSwitch`, rama
  `gingerbread`); `peripheral` devuelve adb/carga por USB. `init.e0.rc` monta debugfs y enlaza la
  propiedad con `/sys/kernel/debug/otg/mode`.
- El teléfono no da 5 V: hace falta un cable OTG con alimentación externa.
- `configs/vold.fstab` monta el disco en `/mnt/usbdisk` (`/devices/platform/msm_hsusb_host.0`).
- El modo host desactiva la detección de VBUS/ID y se pierde al reiniciar. Con host activo el puerto
  USB no sirve para adb: usar adb por WiFi (`setprop service.adb.tcp.port 5555`).
- Ratón y teclado (HID) se ven como dispositivos de entrada normales; ratón probado en CM10.1.

## 📜 Extras

| Prueba                        | Resultado | Comentario                         |
|------------------------------|-----------|------------------------------------|
| Recovery funcional            | ✅        | TWRP inicia correctamente          |
| ADB                           | ✅        | `adb shell` funciona               |
| Root                         | ✅        | `su` da root sin errores           |
| Montaje de particiones        | ✅        | /system, /data y /cache montados   |
> Notas de bring-up, causas raíz y problemas abiertos: [docs/E400_NOTES.md](docs/E400_NOTES.md)
