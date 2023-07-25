// Aquí puedes agregar la ruta (path) de tu archivo local

const filePath = "rx_config.txt";

// Función para cargar el contenido del archivo y mostrarlo en el HTML
function mostrarContenidoArchivo() {
    // Crear una nueva instancia de XMLHttpRequest
    const xhr = new XMLHttpRequest();

    // Configurar el evento de carga para manejar el contenido del archivo
    xhr.onload = function() {
        if (xhr.status === 200) {
            const contenido = xhr.responseText;

            // Dividir el contenido del archivo por líneas
            const lines = contenido.trim().split('\n');

            // Obtener la primera y última línea
            const firstLine = lines[0];
            const lastLine = lines[lines.length - 1];
            // Agregar "Hz" a la primera línea
            const firstLineWithHz = firstLine + " Hz";

            // Agregar "Hz" a la última línea
            const lastLineWithHz = lastLine + " Hz";
            // Mostrar la primera y última línea en los elementos HTML
            document.getElementById('firstLine').innerText += firstLineWithHz;
            document.getElementById('lastLine').innerText += lastLineWithHz;
        } else {
            // Mostrar un mensaje de error en caso de que el archivo no exista o haya algún problema de carga
            console.error('Error al cargar el archivo:', xhr.status, xhr.statusText);
            document.getElementById('firstLine').innerText = 'Primer línea: El archivo no existe o se produjo un error al cargarlo.';
            document.getElementById('lastLine').innerText = 'Última línea: El archivo no existe o se produjo un error al cargarlo.';
        }
    };

    // Configurar el evento de error para manejar problemas de carga
    xhr.onerror = function() {
        console.error('Error de red al cargar el archivo.');
        document.getElementById('firstLine').innerText = 'Primer línea: Error de red al cargar el archivo.';
        document.getElementById('lastLine').innerText = 'Última línea: Error de red al cargar el archivo.';
    };

    // Realizar la petición para obtener el archivo
    xhr.open('GET', filePath);
    xhr.send();
}

// Llamar a la función para cargar y mostrar el contenido del archivo al cargar la página
mostrarContenidoArchivo();