/*$(function() {
  //onLoad
  cmd('stop');
});*/

function cb(data)
{
  //alert( "success: " + data );
  parseJSON(data);
}

function parseJSON(data)
{
  var p = JSON.parse(data);
  $("#pos").text(p.pos);
  $("#prgs").val(p.pos);
  $("#uptm").text(p.uptime);
  $("#buildtm").text(p.buildtime);
  $("#ntptm").text(p.ntptime);
  $("#swver").text(p.swver);
  
  str = "WifiBlinds | "+p.name;
  $(".item1").text(str);
  document.title = str;
}

//chrome.exe --user-data-dir="C://Chrome dev session" --disable-web-security
function cmd(c)
{
  $.get( "http://192.168.1.102/?cmd="+c+"&getdata=pos", cb);
  //$.get( "http://192.168.1.102?getdata=pos", cb);
}
