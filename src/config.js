var basalt_color_val = "";
var debug = false;
var watch_version =-1;

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    if(debug)console.log('PebbleKit JS ready!');
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    if(debug)console.log('AppMessage received!');
    if('basalt_colors' in e.payload){
      basalt_color_val = JSON.stringify(e.payload.basalt_colors);
      basalt_color_val = basalt_color_val.replace('"','');
      basalt_color_val = basalt_color_val.replace('"','');
      if(debug)console.log('AppMessage ' + basalt_color_val +' received!');
    }
    if('watch_version' in e.payload){
      watch_version = JSON.stringify(e.payload.watch_version);
      if(debug)console.log('Watch version is ' + watch_version);
    }
  });
                        
Pebble.addEventListener('showConfiguration', function(e) {
  // Show config page
  if(watch_version == -1){//request data from pebble
    var request = {};
    request.request_data = "Send data plz";
    Pebble.sendAppMessage(request);
  }
  else{
  var options = {};
    options.watch_version = watch_version;
    var url = 'http://centuryglass.github.io/vinewatch/index.html'+
      "?watch_version="+watch_version+
      "&basalt_colors="+basalt_color_val;
    if(debug)console.log('opening '+url);
    Pebble.openURL(url);
  }
});

Pebble.addEventListener('webviewclosed', function(e) {
  // Decode and parse config data as JSON
  var config = JSON.parse(decodeURIComponent(e.response));
  var config_out = {};
  if(debug)console.log('Config window returned: '+ JSON.stringify(config.basalt_colors));
  basalt_color_val = JSON.stringify(config.basalt_colors);
  basalt_color_val = basalt_color_val.replace('"','');
  basalt_color_val = basalt_color_val.replace('"','');
  config_out.basalt_colors = config.basalt_colors;
  Pebble.sendAppMessage(config);
  
});