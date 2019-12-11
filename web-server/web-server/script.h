String SCRIPT = R"=====(

var favorites = [];
var root = "http://" + location.host;
	
$(document).ready(function ( ) {
	initialize();

	$("input[name=red]").on("input change", function () {
		$("#r").text("R: " + $("input[name=red]").val());
		updateColor()
	});

	$("input[name=green]").on("input change", function () {
		$("#g").text(" G: " + $("input[name=green]").val());
		updateColor()
	});
	
	$("input[name=blue]").on("input change", function () {
		$("#b").text(" B: " + $("input[name=blue]").val());
		updateColor()
	});

	$("input[name=brightness]").on("input change", function () {
		$("#brightness").text(" Brightness: " + $("input[name=brightness]").val());
		updateColor()
	});

	$(".color").on("click touchend", function () {
		var red = $("input[name=red]").val();
		var green = $("input[name=green]").val();
		var blue = $("input[name=blue]").val();
		sendColorValues(red, green, blue);
	});
	$(".brightness").on("click touchend", function () {
		sendBrightnessValue();
	});

	$(".addFave").on("click", function (){
		
		var red = $("input[name=red]").val();
		var green = $("input[name=green]").val();
		var blue = $("input[name=blue]").val();
		
		addToFavorites();
		sendFavoriteColor(red, green, blue);
	});

	$(document).on("click", ".favoriteColor", function () {
		var bg_color = $(this).css("background-color");

		$("#color").css("background-color", bg_color);

		var colors = bg_color.replace("rgb(", "").replace(")", "").split(",");

		var red = parseInt(colors[0]);
		var green = parseInt(colors[1]);
		var blue = parseInt(colors[2]);
		
		sendColorValues(red, green, blue);
	});
});

function updateColor() {
	var red = $("input[name=red]").val();
	var green = $("input[name=green]").val();
	var blue = $("input[name=blue]").val();
	var brightness = $("input[name=brightness]").val() / 255;

	var background = "rgb(" + red + ", " + green + ", " + blue + ", " + brightness + ")";

	$("#color").css("background-color", background);
}

function initialize() {
	$("#r").text("R: " + $("input[name=red]").val());
	$("#g").text(" G: " + $("input[name=green]").val());
	$("#b").text(" B: " + $("input[name=blue]").val());
	$("#brightness").text(" Brightness: " + $("input[name=brightness]").val());

	updateColor();
}

function sendColorValues(red, green, blue) {
	var url = root + "/changeColor";
	$.ajax({
		url : url,
		data: {red : red, green : green, blue : blue},
		type: 'post'
	});
}

function sendBrightnessValue() {
	var brightness = $("input[name=brightness]").val();
	
	var url = root + "/changeBrightness";

	$.ajax({
		url : url,
		data: {brightness : brightness},
		type: 'post'
	});
}

function addToFavorites() {
	var red = $("input[name=red]").val();
	var green = $("input[name=green]").val();
	var blue = $("input[name=blue]").val();

	favorites.push([red, green, blue]);
	
	var index = favorites.length;

	var element = "<button class='favoriteColor' id='" + index + "'></button>"
	
	var background = "rgb(" + red + ", " + green + ", " + blue + ")";

	$(".favorites").append(element);

	$("#" + index).css("background-color", background);
}

function sendFavoriteColor (red, green, blue) {
	var url = root + "/addToFavorite";

	$.ajax({
		url : url,
		data: {red : red, green : green, blue : blue},
		type: 'post'
	});
}
)=====";