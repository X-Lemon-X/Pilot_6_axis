const char MAIN_page_beg[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>HTML Form to Input Data</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {font-family: Times New Roman; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem; color: #FF0000;}
  </style>
  </head><body>
  <h2>HTML Form to Input Data</h2> 

  <form action="/get">
  <table border="1">
  <tbody>
  
  <tr>
      <td>VERIABLES</td>
      <td>VALUES</td>
  </tr>)rawliteral";

  const char MAIN_page_end[] PROGMEM = R"rawliteral(
</tbody>
</table>
<input type="submit" value="Save">
</form>
</body></html>)rawliteral";