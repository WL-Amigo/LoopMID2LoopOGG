// from: https://www.w3docs.com/snippets/javascript/how-to-detect-internet-explorer-in-javascript.html
//detects if user is using Internet Explorer
//returns version of IE or false, if browser is not IE
//Function to detect IE or not
function isIE() {
  var ua = window.navigator.userAgent;
  var msie = ua.indexOf('MSIE ');
  if (msie > 0) {
    // IE 10 or older
    return true;
  }
  var trident = ua.indexOf('Trident/');
  if (trident > 0) {
    // IE 11
    return true;
  }

  // User uses other browser
  return false;
}

if(isIE()) {
  var ieWarning = document.createElement('div');
  ieWarning.className = "ie-warning";
  ieWarning.innerText = "本サイトは Internet Explorer では正常動作しません。Firefox, Chrome, Edge 等で御覧下さい。";
  document.body.appendChild(ieWarning);
}