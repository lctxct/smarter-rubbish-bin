// ===== NOTE! =====
// DO NOT FILL YOUR DETAILS IN BELOW. 
// DUPLICATE THIS TEMPLATE AND CREATE A NEW 
// "secrets.h" and "wifi_secrets.h" FILE. 
// THIS FILE HAS NOT BEEN .gitignore-d. 

// ===== config =====
// Set as 1 if using school wifi 
#define SCHOOL_WIFI 0

// ===== mqtt secrets =====
// Replace ip address with ip of your windows 
// Click "properties" of the wifi you're currently connected to 
// and look for the ipv4 address
#define SERVER "mqtt://0.0.0.0:1883"

// ===== wifi secrets =====
#define SSID ""
#define PASSWD ""

// ===== school wifi secrets =====
#define NUS_NET_IDENTITY "nusstu\exxxxxxx"  //ie nusstu\e0123456
#define NUS_NET_USERNAME "exxxxxx"
#define NUS_NET_PASSWORD ""
