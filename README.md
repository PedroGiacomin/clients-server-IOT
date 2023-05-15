# clients-server-IOT

## Topologia
O sistema simulado é uma rede com topologia em barramento, i.e., todos os nós estão conectados fisicamente uns aos outros pelo mesmo barramento. O protocolo de enlace utilizado é o CSMA, provido pela classe `CsmaHelper` do NS-3. 

A topologia da aplicação é *cliente-servidor*, sendo que:
- Há `[numClients]` clientes, definido no código;
- O servidor é sempre o **último** nó (ou seja, o nó de index `[numClients]`);
- Todos estão na mesma sub-rede, e seus IPs são `10.0.0.x`

## Aplicação
A aplicação consiste em todos os clientes enviarem pacotes de PING ao servidor e a todos os outros clientes na rede. Assim, todos os clientes se comunicam entre si e com o servidor. Foi usada a aplicação *ping* do NS-3, provida pela classe `V4PingHelper`. 

Cada cliente possui `[numClients]` aplicações ping funcionando simultaneamente, pois cada aplicação só se comunica diretamente com um endereço de IP.

### Resumo do funcionamento
Consideremos o cliente **n** na rede com **[numClients]** clientes.
1. No instante de tempo `1.0 + n` o cliente inicia suas aplicações *ping*.
2. São enviados os pacotes de ping (request) a cada um dos clientes e ao servidor, respeitando o atraso atribuído ao enlace.
3. O cliente recebe os pacotes de ping (reply) de cada um dos clientes e do servidor.
4. O cliente espera `[numClients]` segundos para enviar o próximo pacote.
5. Nesse meio tempo, o cliente recebe, de 1 em 1 segundo, um pacote de ping (request) de cada um dos demais clientes, respondendo imediatamente com um pacote de ping (reply).
6. Não são recebidos pacotes de ping (resquest) do servidor, pois a aplicação não está instalada nele.

### Versionamento
- **v1** -> Os clientes pingam no cliente e no servidor ao mesmo tempo, isto é, as aplicações todas iniciam simultaneamente.
- **v2** -> Os clientes pingam primeiro no servidor, para depois pingar nos demais clientes. (A aplicação de ping no servidor começa *0.5 segundo* mais cedo).
