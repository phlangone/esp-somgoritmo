"""
Servidor HTTP para receber e salvar dados de áudio no formato WAV.

Este script implementa um servidor HTTP que recebe dados de áudio via requisições POST
e salva esses dados em arquivos WAV. O servidor suporta transferências de dados
utilizando a codificação de chunks (chunked transfer encoding) e pode extrair informações
sobre a taxa de amostragem, bits por amostra e número de canais dos headers HTTP.

Dependendo da versão do Python em execução (Python 2 ou Python 3), o script utiliza
módulos e funções específicos para manipulação de URLs e servidores HTTP.

Exemplo de uso:
    python script_name.py --ip <IP_ADDRESS> --port <PORT_NUMBER>

Parâmetros:
    --ip, -i   : Endereço IP no qual o servidor irá escutar. Se não for fornecido, o IP local será usado.
    --port, -p : Porta na qual o servidor irá escutar. Se não for fornecido, a porta padrão é 8000.
"""

import os
import datetime
import sys
import wave
import argparse
import socket

if sys.version_info.major == 3:
    # Python 3: Importa módulos para manipulação de URLs e servidores HTTP.
    from urllib import parse
    from http.server import HTTPServer, BaseHTTPRequestHandler
else:
    # Python 2: Importa módulos equivalentes para manipulação de URLs e servidores HTTP.
    import urlparse
    from BaseHTTPServer import HTTPServer, BaseHTTPRequestHandler

PORT = 8000  # Porta padrão para o servidor HTTP

class Handler(BaseHTTPRequestHandler):
    """
    Classe que lida com requisições HTTP, herda de BaseHTTPRequestHandler.
    
    Implementa os métodos necessários para processar requisições POST que contenham
    dados de áudio e salvá-los em arquivos WAV, bem como processar requisições GET básicas.
    """

    def _set_headers(self, length):
        """
        Define os cabeçalhos HTTP da resposta.

        :param length: Comprimento do conteúdo da resposta.
        """
        self.send_response(200)
        if length > 0:
            self.send_header('Content-length', str(length))
        self.end_headers()

    def _get_chunk_size(self):
        """
        Lê o tamanho do chunk da requisição HTTP.

        :return: Tamanho do chunk em bytes.
        """
        data = self.rfile.read(2)
        while data[-2:] != b"\r\n":
            data += self.rfile.read(1)
        return int(data[:-2], 16)

    def _get_chunk_data(self, chunk_size):
        """
        Lê os dados do chunk da requisição HTTP.

        :param chunk_size: Tamanho do chunk a ser lido.
        :return: Dados do chunk.
        """
        data = self.rfile.read(chunk_size)
        self.rfile.read(2)
        return data

    def _write_wav(self, data, rates, bits, ch):
        """
        Salva os dados recebidos em um arquivo WAV.

        :param data: Dados de áudio a serem salvos.
        :param rates: Taxa de amostragem do áudio.
        :param bits: Número de bits por amostra.
        :param ch: Número de canais de áudio.
        :return: Nome do arquivo WAV salvo.
        """
        t = datetime.datetime.utcnow()
        time = t.strftime('%Y%m%dT%H%M%SZ')
        filename = str.format('{}_{}_{}_{}.wav', time, rates, bits, ch)

        wavfile = wave.open(filename, 'wb')
        wavfile.setparams((ch, int(bits/8), rates, 0, 'NONE', 'NONE'))
        wavfile.writeframesraw(bytearray(data))
        wavfile.close()
        return filename

    def do_POST(self):
        """
        Processa requisições POST para salvar dados de áudio em um arquivo WAV.
        """
        if sys.version_info.major == 3:
            urlparts = parse.urlparse(self.path)
        else:
            urlparts = urlparse.urlparse(self.path)

        request_file_path = urlparts.path.strip('/')
        total_bytes = 0
        sample_rates = 0
        bits = 0
        channel = 0
        print("Processando POST...")

        if (request_file_path == 'upload'
            and self.headers.get('Transfer-Encoding', '').lower() == 'chunked'):
            data = []
            sample_rates = self.headers.get('x-audio-sample-rates', '').lower()
            bits = self.headers.get('x-audio-bits', '').lower()
            channel = self.headers.get('x-audio-channel', '').lower()

            print("Informações de áudio, taxa de amostragem: {}, bits: {}, canais: {}".format(sample_rates, bits, channel))
            
            while True:
                chunk_size = self._get_chunk_size()
                total_bytes += chunk_size
                print("Bytes totais recebidos: {}".format(total_bytes))
                sys.stdout.write("\033[F")
                if chunk_size == 0:
                    break
                else:
                    chunk_data = self._get_chunk_data(chunk_size)
                    data += chunk_data

            filename = self._write_wav(data, int(sample_rates), int(bits), int(channel))
            self.send_response(200)
            self.send_header("Content-type", "text/html;charset=utf-8")
            self.send_header("Content-Length", str(total_bytes))
            self.end_headers()
            body = 'Arquivo {} foi salvo, tamanho {}'.format(filename, total_bytes)
            self.wfile.write(body.encode('utf-8'))

    def do_GET(self):
        """
        Processa requisições GET básicas.
        """
        print("Processando GET")
        self.send_response(200)
        self.send_header('Content-type', "text/html;charset=utf-8")
        self.end_headers()

def get_host_ip():
    """
    Obtém o IP do host local.

    :return: IP do host local.
    """
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(('8.8.8.8', 80))
        ip = s.getsockname()[0]
    finally:
        s.close()
    return ip

parser = argparse.ArgumentParser(description='Servidor HTTP que salva dados de áudio em arquivos WAV.')
parser.add_argument('--ip', '-i', nargs='?', type=str)
parser.add_argument('--port', '-p', nargs='?', type=int)
args = parser.parse_args()

if not args.ip:
    args.ip = get_host_ip()
if not args.port:
    args.port = PORT

httpd = HTTPServer((args.ip, args.port), Handler)

try:
    print("Servindo HTTP em {} na porta {}".format(args.ip, args.port))
    httpd.serve_forever()
except KeyboardInterrupt:
    print("\nServidor interrompido pelo usuário")
    httpd.server_close()  # Fecha o servidor de forma limpa
