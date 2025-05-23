#pragma once


const char MAIN_page_style[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>RC 6D</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {
      font-family: 'Segoe UI', 'Roboto', Arial, sans-serif;
      display: inline-block;
      text-align: center;
      font-size: 16px; /* base for rem/em scaling */
    }
    h2 {
      font-size: 2.2rem;
      color: #0bcfe4;
      font-family: inherit;
      font-weight: 700;
      letter-spacing: 0.09em;
      margin-top: 2rem;
      margin-bottom: 1.7rem;
      text-shadow: 0 0.125em 0.5em rgba(11,207,228,0.08);
    }
    body {
      background-color: #1c1c1c;
      color: white;
      margin: 0;
      padding: 0;
    }
    form {
      width: 98vw;
      max-width: 600px;
      margin: 0 auto;
    }
    table {
      border-collapse: collapse;
      width: 100%;
      margin: 0 auto 1.25em auto;
      background-color: #232323;
      border-radius: 0.625em;
      overflow: hidden;
      box-shadow: 0 0.125em 0.5em rgba(0,0,0,0.4);
      font-size: 1em;
    }
    th, td {
      padding: 0.75em 0.625em;
      text-align: left;
      font-size: 1em;
    }
    tr {
      border-bottom: 1px solid #333;
      transition: background 0.2s;
    }
    tr:hover {
      background-color: #292929;
    }
    td:first-child {
      font-weight: bold;
      color: white;
      width: 40%;
      font-size: 1em;
    }
    input[type="text"] {
      width: 95%;
      padding: 0.45em;
      border: 1px solid #444;
      border-radius: 0.3125em;
      background-color: #181818;
      color: #fff;
      font-size: 1em;
      outline: none;
      transition: border 0.2s;
      box-sizing: border-box;
    }
    input[type="text"]:focus {
      border: 0.09375em solid #0bcfe4;
      background-color: #222;
    }
    .section-header {
      background-color: #181818;
      height: 1.125em;
      border-bottom: 0.125em solid white;
      text-align: center;
    }
    .section-header span {
      color: white;
      font-weight: bold;
      letter-spacing: 0.06em;
      font-size: 1.1em;
    }
    .save-btn {
      display: block;
      width: 100%;
      max-width: 95vw;
      margin: 1.9em auto 0 auto;
      padding: 1em 0;
      font-size: 1.1em;
      font-weight: bold;
      color: black;
      background: linear-gradient(90deg, white 0%, white 100%);
      border: none;
      border-radius: 0.5em;
      box-shadow: 0 0.125em 0.5em rgba(11,207,228,0.2);
      cursor: pointer;
      transition: background 0.2s, transform 0.1s;
      letter-spacing: 0.06em;
    }
    input[type="password"] {
      width: 95%;
      padding: 0.45em;
      border: 1px solid #444;
      border-radius: 0.3125em;
      background-color: #181818;
      color: #fff;
      font-size: 1em;
      outline: none;
      transition: border 0.2s;
      box-sizing: border-box;
    }
    input[type="password"]:focus {
      border: 0.09375em solid #0bcfe4;
      background-color: #222;
    }

    .show-password-btn {
      display: inline-block;
      padding: 0.7em 1.5em;
      font-size: 0.98em;
      font-weight: bold;
      color: black;
      background: linear-gradient(90deg, white 0%, white 100%);
      border: none;
      border-radius: 0.5em;
      box-shadow: 0 0.125em 0.5em rgba(11,207,228,0.08);
      cursor: pointer;
      transition: background 0.2s, transform 0.1s;
      letter-spacing: 0.06em;
      margin-top: 0.5em;
      width: 100%;
      margin: 0.5em auto 0 auto;
      text-align: center;
    }

    @media (max-width: 600px) {
      html { font-size: 15px; }
      form { max-width: 99vw; }
      table { font-size: 0.97em; }
      .save-btn { font-size: 1em; }
      th, td { padding: 0.6em 0.4em; }
    }
    @media (max-width: 600px) {
      html { font-size: 13px; }
      table { font-size: 0.93em; }
      .save-btn { font-size: 0.95em; }
    }
  </style>
</head>
<body>
)rawliteral";

const char SETTING_page_end[] PROGMEM = R"rawliteral(
  <button onclick="window.location.href='/'" class="back-btn" style="display:inline-block;margin:1em auto 2em auto;padding:0.7em 2em;font-size:1em;font-weight:bold;color:#0bcfe4;background:#232323;border:none;border-radius:0.5em;box-shadow:0 0.1em 0.3em rgba(11,207,228,0.12);text-decoration:none;transition:background 0.2s, color 0.2s;cursor:pointer;">
    &larr; Back
  </button>
  <script>
    setTimeout(function() {
      window.location.href = '/';
    }, 3000);
  </script>

</body>
</html>

)rawliteral";

const char MAIN_table_separator[] PROGMEM = R"rawliteral(
       
<tr>
  <td colspan="2" class="section-header">
    <span>WIFI target networks</span>
  </td>
</tr>

<tr>
  <td colspan="2" style="text-align:right;">
    <button type="button" class="show-password-btn" onclick="toggleAllPasswords(this)">Show Passwords</button>
  </td>
</tr>

)rawliteral";

const char MAIN_page_beg[] PROGMEM = R"rawliteral(
  <h2>Remote Controller 6D</h2>
  <form action="/get">
    <table>
      <tbody>
        <tr>
          <td colspan="2" class="section-header">
            <span>General Settings</span>
          </td>
        </tr>

)rawliteral";

const char MAIN_page_end[] PROGMEM = R"rawliteral(
        <script>
          function toggleAllPasswords(btn) {
            var show = btn.textContent === "Show Passwords";
            var inputs = document.querySelectorAll('input#wifipass');
            inputs.forEach(function(input) {
              input.type = show ? "text" : "password";
            });
            btn.textContent = show ? "Hide Passwords" : "Show Passwords";
          }
        </script>

      
      </tbody>
    </table>
    <input type="submit" value="Save" class="save-btn">
    <p style="color: #ffb300; margin-top: 1.2em; font-size: 1em;">
      <strong>Note:</strong> Saving these settings will restart the MCU.
    </p>
  </form>
</body>
</html>

)rawliteral";