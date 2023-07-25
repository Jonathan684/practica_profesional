
var selectedTest1 = '1';
var selectedTest2 = '0';
var selectedLoopback1 = '0';
var selectedLoopback2 = '1';

var defaultValues = {
   ip: "", // Set your default IP address here
   test1: "1", // Set your default test1 value here
   test2: "0", // Set your default test2 value here
   loop1: "0", // Set your default loop1 value here
   loop2: "1"  // Set your default loop2 value here
};


function selectTest(test) {
   if(test == "1"){
      selectedTest1 = "1";
      selectedTest2 = "0";
   }
   if(test == "2"){
      selectedTest1 = "0";
      selectedTest2 = "1";
   }
   console.log("Selected Test: "+ selectedTest1 +" "+selectedTest2);
   //console.log("Selected Test: " + selectedTest);
}

function selectLoopback(loopback) {
   if(loopback == "1"){
      selectedLoopback1 = "1";
      selectedLoopback2 = "0";
   }
   if(loopback == "2"){
      selectedLoopback1 = "0";
      selectedLoopback2 = "1";
   }
   console.log("Selected loop: "+ selectedLoopback1 +" "+selectedLoopback2);
}


function ejecutarScript() {
    var ip = document.getElementById("ipInput").value;
    // Crear una nueva solicitud HTTP
    var xhr = new XMLHttpRequest();
    xhr.open("POST", "/ejecutar_sh", true);
    xhr.setRequestHeader("Content-Type", "application/json");
    
   //  // Enviar la dirección IP y los valores de Test y Loopback como datos JSON
    var data = {
      ip: ip,
      test1: selectedTest1,
      test2: selectedTest2,
      loop1: selectedLoopback1, // Agregar el valor del loopback aquí
      loop2: selectedLoopback2 // Agregar el valor del loop1 aquí
   };
   //console.log("ip "+ip+"test1 "+ selectedTest1 +" "+selectedTest2+"loop1: "+ selectedLoopback1 +"loop2: "+selectedLoopback2);
   //console.log(data);
   
   // console.log(data);
   xhr.send(JSON.stringify(data));

    // Escuchar los eventos de carga y error de la solicitud
    xhr.onload = function() {
      if (xhr.status === 200) {
         console.log("Archivo .sh ejecutado correctamente.");
      } else {
         console.log("Error al ejecutar el archivo .sh:", xhr.statusText);
      }
      
    };
    xhr.onerror = function() {
    console.log("Error de conexion al ejecutar el archivo .sh.");
   };
}





document.addEventListener("DOMContentLoaded", function() {
   var fecha = new Date(); 
   var hora = fecha.getHours();
   var minutos = fecha.getMinutes();

   if (minutos < 10) {
   minutos = "0" + minutos;
   }

   var horaActual = hora + ":" + minutos;
   document.getElementById("hora").textContent = "Hora actual: " + horaActual;
 });



// Cargar los valores del archivo default_values.json y establecerlos como valores por defecto en los campos de entrada
document.addEventListener("DOMContentLoaded", function() {
   var ipInput = document.getElementById("ipInput");
   var test1 = document.getElementById("test1");
   var test2 = document.getElementById("test2");

   var loop1 = document.getElementById("loop1");
   var loop2 = document.getElementById("loop2");

   
   // var testRadios = document.querySelectorAll('input[name="test"]');
   // var loopRadios = document.querySelectorAll('input[name="loop"]');

   // Crear una nueva solicitud HTTP para cargar el contenido del archivo default_values.json
   var xhr = new XMLHttpRequest();
   xhr.open("GET", "/valores.json", true);

   // Enviar la solicitud HTTP
   xhr.send();

   // Escuchar los eventos de carga y error de la solicitud
   xhr.onload = function() {
       if (xhr.status === 200) {
           // Si la solicitud fue exitosa, obtener los valores del archivo JSON
           var jsonData = JSON.parse(xhr.responseText);

           // Establecer los valores del JSON en los campos de entrada correspondientes
           ipInput.value = jsonData.ip;
           // Establecer la selección según los valores de jsonData
            if (jsonData.test1 === "1") {
               test1.checked = true;
            } else if (jsonData.test2 === "1") {
               test2.checked = true;
            }
            if (jsonData.loop1 === "1") {
               loop1.checked = true;
            } else if (jsonData.loop2 === "1") {
               loop2.checked = true;
            }
           //loop1.value = jsonData.loop1;
           //console.log("jsonData.test1 "+jsonData.test1);
           //loop2.value = jsonData.loop2;
           //test1.value = jsonData.test1;
           //test2.value = jsonData.test2;

       } else {
           console.log("Error al cargar los valores por defecto:", xhr.statusText);
       }
   };
   xhr.onerror = function() {
       console.log("Error de conexión al cargar los valores por defecto.");
   };
});

