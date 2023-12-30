const char MAIN_page_beg[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>RC 6D</title>
  <body style="background-color:#1c1c1c; color: white;"></body>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {font-family: Times New Roman; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem; color: #0bcfe4;}
  </style>
  </head><body>
  <h2>Remote Controler 6D Setings</h2> 

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
<input type="submit" value="Save" >
</form>
</body></html>)rawliteral";