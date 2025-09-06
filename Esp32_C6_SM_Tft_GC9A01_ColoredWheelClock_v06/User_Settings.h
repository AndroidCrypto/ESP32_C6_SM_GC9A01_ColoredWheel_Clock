/*
  This file holds all settings for this project.
*/

// --------------------------------------------------------------
// Wi-Fi

//const char* ssid     = "your_SSID";
//const char* password = "your_PASSWORD";
// Change to your WiFi credentials

// --------------------------------------------------------------
// Timezone Information
const char* TIMEZONE    = "CET-1CEST,M3.5.0,M10.5.0/3"; // Choose your time zone from: https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv 
                                                        // See below for examples
const char* TIMEZONE_GMT = "GMT0";                      // we need the GMT time for building the Meteomatics call
const char* NTP_SERVER   = "pool.ntp.org";              // Or, choose a time server close to you, but in most cases it's best to use pool.ntp.org to find an NTP server
                                                        // then the NTP system decides e.g. 0.pool.ntp.org, 1.pool.ntp.org as the NTP syem tries to find  the closest available servers
                                                        // EU "0.europe.pool.ntp.org"
                                                        // US "0.north-america.pool.ntp.org"
                                                        // See: https://www.ntppool.org/en/  
const char* NTP_SERVER_2 = "time.nist.gov";                                                           

// --------------------------------------------------------------
/* Example time zones
const char* Timezone = "MET-1METDST,M3.5.0/01,M10.5.0/02"; // Most of Europe
const char* Timezone = "CET-1CEST,M3.5.0,M10.5.0/3";       // Central Europe
const char* Timezone = "EST-2METDST,M3.5.0/01,M10.5.0/02"; // Most of Europe
const char* Timezone = "EST5EDT,M3.2.0,M11.1.0";           // EST USA  
const char* Timezone = "CST6CDT,M3.2.0,M11.1.0";           // CST USA
const char* Timezone = "MST7MDT,M4.1.0,M10.5.0";           // MST USA
const char* Timezone = "NZST-12NZDT,M9.5.0,M4.1.0/3";      // Auckland
const char* Timezone = "EET-2EEST,M3.5.5/0,M10.5.5/0";     // Asia
const char* Timezone = "ACST-9:30ACDT,M10.1.0,M4.1.0/3":   // Australia
*/