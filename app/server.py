from http.server import HTTPServer, SimpleHTTPRequestHandler
import subprocess
import json

# Nombre del archivo donde se guardarán los valores seleccionados
archivo_valores = "valores.json"
state_init = 0

def init(ip):
    result = subprocess.run(['./init_config.sh',ip])
    # Verificar el código de retorno del proceso
    
    if result.returncode == 0:
        # Si el script se ejecutó correctamente, iniciar el servidor
        print('Script init_config.sh ejecutado correctamente. Se iniciará el servidor.')
    else:
        # Si hubo un error al ejecutar el script, mostrar un mensaje de error
        print('Error al ejecutar init_config.sh. No se iniciará el servidor.')
    

class RequestHandler(SimpleHTTPRequestHandler):
    def do_POST(self):
        global state_init

        content_length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(content_length)
        data_str = post_data.decode('utf-8')
        
        #print("data_str", data_str)
        data = json.loads(data_str)
        guardar_valores(data)
        ip = data.get('ip', '')
        test1 = data.get('test1', '')
        test2 = data.get('test2', '')
        loop1 = data.get('loop1', '') # Obtener el valor de 'loopback'
        loop2 = data.get('loop2', '') # Obtener el valor de 'loopback'
       
        if test1 == '1':
            test = '1'
        if test1 == '0':
            test = '2' 
             
        if loop1 == '1':
            loop = '0'
        if loop1 == '0':
            loop = '1'    
        #print("state_init :",state_init)
        "si es el estado inicial debo cargar todo o si no coicide con el ip guardado tambien debe recargar"
        with open("ip.txt", "r") as archivo:
            ip_anterior = archivo.readline().strip()
        
        if state_init == 0 or ip != ip_anterior:
            print("Cargar por primera vez o IP diferente.")
            init(ip)
            state_init = 1
        
        # Ejecutar el archivo .sh pasando la dirección IP como argumento
        subprocess.run(['./script.sh', ip,test,loop])
        # Guardar los valores seleccionados en el archivo JSON

        self.send_response(200)
        self.send_header('Content-type', 'text/plain')
        self.end_headers()
        self.wfile.write(b'Archivo .sh ejecutado correctamente')
        


# Función para guardar los valores seleccionados en el archivo JSON
def guardar_valores(data):
    with open(archivo_valores, "w") as archivo:
        json.dump(data, archivo)

def run():
    server_address = ('', 8001)
    httpd = HTTPServer(server_address, RequestHandler)
    print('Servidor en ejecución en http://localhost:8001')
    state_init = 0
    httpd.serve_forever()

if __name__ == '__main__':
    run()
