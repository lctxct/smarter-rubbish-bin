# CS3237 Introduction to Internet of Things AY2023/24 Sem 1
**Group 6**

This 🗑️ is smarter than your future.

## Standardize 
Table name: **bintable**

| col_name   | type   | remarks |
|------------|--------|---------|
| id         |        | auto    |
| created_at |        | auto    |
| photo_path | text   |         |
| class      | int4   |         |
| fill_r     | float8 |         |
| fill_t     | float8 |         |

**MQTT Topics**

| topic                 | description |
|-----------------------|-------------|
| fill_level/recyclable |             |
| fill_level/trash      |             |

## Table of contents
- [ai/](#ai)
- [documents/](#documents)
- [esp32/](#esp32)
- [local/](#local)
- [phil_bot/](#phil_bot)
- [test_mqtt/](#test_mqtt)
- [test_postgres/](#test_postgres)

---

## ai/
Dataset obtained from https://github.com/garythung/trashnet.

---

## documents/
Contains things like project proposal. 

---

## esp32/
PlatformIO! Installation instructions [here](https://platformio.org/install/ide?install=vscode). (it's just installation of a vscode extension). Might have to do some other installation stuff for ESP32. 

Some source code notes: 
- `src/main.cpp` contains the sketch. To use in Arduino's IDE, can just copy and paste the code. Remove the `#include "Arduino.h"` (first line). 
- `include/` should contain the header files to be used in `main.cpp`. `secrets_template.h` contains the values that need to be defined. Be careful not to populate it with any secreets and accidentally push. The current `main.cpp` code requires `secrets.h` and `wifi_secrets.h` which is just the stuff in the template. Feel free to comment out anything you don't need. 

Some PlatformIO notes: 
- Click on PlatformIO icon on the left-hand navbar to start
- You should be able to see more options on the vscode bottom bar. We probably only have to worry about `Build`, `Upload` and `Serial Monitor`. 

---

## local/
Sorry. Setting up will be quite a pain. Once everything is done, `python3 smartbin.py`. 

1. [Secrets](#secrets)
2. [Setting up](#setting-up)
3. [PostgreSQL](#postgresql)
4. [MQTT](#mqtt)

### Secrets 
- Create `.env` by renaming `.env.template` -> `.env`. Fill values. (ask from me)
- Create `secrets/` folder. 
- `secrets/` should contain
    - `smart-rubbish-bin-firebase-adminsdk.json` (ask from me)
    - `database.ini` file should have the following
    ```
    [postgresql]
    host=localhost
    database=bindb
    user=postgres
    password=<YOUR_PASSWORD_HERE>
    port=5432
    ```

### Setting up
Assumes that you start out in the `local/` directory. 
Create a virtual environment
```
python3 -m venv venv
```
To start the virtual env
```
source venv/bin/activate 
```
To get out
```
deactivate
```
Install dependencies
```
pip install -r requirements.txt
```

### PostgreSQL
Follow instructions from online. 

### MQTT
```
mosquitto -c /etc/mosquitto/mosquitto.conf
```

---

## phil_bot/ 
Philbert the Telegram Fill Bot! Tells you how fill he is. 

**Commands:**
- `/fill` - Checks fill level

---

## test_mqtt/
For testing WiFi + MQTT from ESP32. :) Remember to set `#define`nations. For wsl + mqtt support, refer to: [link](https://gist.github.com/lctxct/50692f1761ea4c3773417e7c2c07a597).

### Includes
- `test_mqtt.ino`
- `mqtt_listen.py`

### Usage 
1. `python3 mqtt_listen.py` 
2. Upload `test_mqtt.ino` to ESP32

---

## test_postgres/ 

Collection of PostgreSQL notes. 
