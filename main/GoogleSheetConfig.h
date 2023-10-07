// paste the contents from GoogleSheetConfig.txt here
// google sheet configuration file
// defines the google sheet to pull data from

#ifndef GOOGLE_SHEET_CONFIG_H
#define GOOGLE_SHEET_CONFIG_H

String GOOGLE_SHEET_BASE_API = "https://sheets.googleapis.com/v4/spreadsheets/";
String GOOGLE_SHEET_ID = "";
String GOOGLE_SHEET_API_ENDPOINT = GOOGLE_SHEET_BASE_API + GOOGLE_SHEET_ID;
String GOOGLE_SHEET_API_KEY = ""; 

// range of cells to get data from (A1:Z gets all cells)
String GOOGLE_SHEET_CELL_RANGE = "A1:Z"; 

// api endpoint to request google sheet data
String GOOGLE_SHEET_REQUEST_URL = GOOGLE_SHEET_API_ENDPOINT + "/values/" + GOOGLE_SHEET_CELL_RANGE + "?key=" + GOOGLE_SHEET_API_KEY;

#endif
