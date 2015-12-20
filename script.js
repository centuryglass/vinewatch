$(function () {
  $('#first-color').on('click', function (event) {
	changeFirst = true;
	changeSecond = false;
	changeThird = false;
	highlightColorBox($(this));
  });
});

$(function () {
  $('#second-color').on('click', function (event) {
	changeFirst = false;
	changeSecond = true;
	changeThird = false;
	highlightColorBox($(this));
  });
});

$(function () {
  $('#third-color').on('click', function (event) {
	changeFirst = false;
	changeSecond = false;
	changeThird = true;
	highlightColorBox($(this));
  });
});

$(function () {
  $('#color-picker .color-box').on('click', function (event) {
	var selected_color = rgb2hex($(this).css("background-color"));

	if (changeFirst)
		$('#first-color').css('background-color', selected_color);
	else if(changeSecond)
		$('#second-color').css('background-color', selected_color);
	else
		$('#third-color').css('background-color', selected_color);
	$( "#color-picker" ).popup( "close" );   
  });
});



$(function () {
	$("#cancel").click(function() {
		console.log("Cancelling");
		document.location = "pebblejs://close";
	});
});

$(function () {
	$("#save").click(function() {
		console.log("Submit");

		var options = saveOptions();
		var return_to = getQueryParam('return_to', 'pebblejs://close#');
		var location = return_to + encodeURIComponent(JSON.stringify(options));
		console.log("Warping to:");
		console.log(location);
		document.location = location;
	});
});

function highlightColorBox($colorButton) {
	selectedColor = rgb2hex($colorButton.css("background-color"));
	
	var i;
	for (i = 1; i <= 13; i++)
	{
		$('#color-row'+i).children().removeClass("selected-color-box");
	}
	$('#color-box-'+selectedColor.substring(1).toUpperCase()).addClass("selected-color-box");
}

//Function to convert hex format to a rgb color
function rgb2hex(rgb){
 rgb = rgb.match(/^rgb\((\d+),\s*(\d+),\s*(\d+)\)$/);
 return ("#" +
  ("0" + parseInt(rgb[1],10).toString(16)).slice(-2) +
  ("0" + parseInt(rgb[2],10).toString(16)).slice(-2) +
  ("0" + parseInt(rgb[3],10).toString(16)).slice(-2)).toUpperCase();
}


function getQueryParam(variable, default_) {
    var query = location.search.substring(1);
    var vars = query.split('&');
    for (var i = 0; i < vars.length; i++) {
        var pair = vars[i].split('=');
        if (pair[0] == variable)
            return decodeURIComponent(pair[1]);
    }
    return default_ || false;
}

function isNumber(n) {
	return !isNaN(parseFloat(n)) && isFinite(n);
}


function saveOptions() {

	var options = {};
	
	
	var colors = rgb2hex($('#first-color').css("background-color")).replace("#", '');
	colors = colors + rgb2hex($('#second-color').css("background-color")).replace("#", '');
	colors = colors + rgb2hex($('#third-color').css("background-color")).replace("#", '');
	options.basalt_colors = colors;
	
	return options;
}

$("document").ready(function() {
	var basalt_colors = getQueryParam("basalt_colors", 0);
	basalt_colors = basalt_colors.length == 18 ? basalt_colors : "000000AAAAAAFFFFFF";
	$('#first-color').css('background-color', "#"+basalt_colors.substring(0,6));
	$('#second-color').css('background-color', "#"+basalt_colors.substring(6,12));
	$('#third-color').css('background-color', "#"+basalt_colors.substring(12,18));
	$('#color1').removeClass("hidden");
	$('#color2').removeClass("hidden");
	$('#color3').removeClass("hidden");

});
