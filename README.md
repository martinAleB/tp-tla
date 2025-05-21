[![✗](https://img.shields.io/badge/Release-v1.1.0-ffb600.svg?style=for-the-badge)](https://github.com/agustin-golmar/Flex-Bison-Compiler/releases)

[![✗](https://github.com/agustin-golmar/Flex-Bison-Compiler/actions/workflows/pipeline.yaml/badge.svg?branch=production)](https://github.com/agustin-golmar/Flex-Bison-Compiler/actions/workflows/pipeline.yaml)

# Aclaraciones sobre el trabajo

## Cambios en las construcciones

Se realizaron ciertas modificaciones sobre las construcciones originales.

* La cláusula "attributes" es ahora opcional (su ausencia se interpreta como '*'), quedando "from" como la única obligatoria.
* En las cláusulas "from" y "group_by" también es posible guardar objetos para elegir un alias para las tablas.
* La cláusula "attributes" es un array de objetos que contienen obligatoriamente el nombre de un atributo, pudiendo espcificar una tabla a la que referencia, un alias y una funcion de agregación a ser aplicada al mismo.
* La cláusula "where" es ahora un array cuyo primer elemento es una condición, mientras que los subsiguientes son objetos AND y OR.
* Los objetos AND y OR también aceptan ser un array del mismo estilo que se definió para la cláusula where, o bien pueden ser simplemente condiciones.
* La cláusula "having" está implícita dentro del "where". Las condiciones dentro de este último que contengan funciones de agregación serán ubicadas en el "having" desde el backend.
* La cláusula "order_by" permite especificar atributos con la opción de añadir funciones de agregación y un tipo de orden (ASC o DESC).
* Se agregó la cláusula auxiliary, dentro de la cual se definen variables que guardan consultas.

## Cambios en los casos de aceptación/rechazo

* El caso de rechazo "Un programa que tenga 2 atributos duplicados en una misma consulta" fue descartado ya que se descubrió que dicho caso es aceptado en PostgreSQL. En consecuencia fue reemplazado por  siguiente caso: "Un programa que contenga cláusula 'where' con una condición de tipo 'IN' sin ningún atributo". Este último se debe chequear en el backend, razón por la que todavía es aceptado aunque debería rechazarse.
* El caso de rechazo "Un programa que no contenga las cláusulas obligatorias (attributes y from)" fue reemplazado por: "Un programa que no contenga la cláusula obligatoria 'from'" ya que "attributes" no es más obligatoria. Además dicha ausencia del from se chequea en el backend, por lo que el test es aceptado aunque debería rechazarse.

# Flex-Bison-Compiler

A base compiler example, developed with Flex and Bison.

* [Environment](#environment)
* [CI/CD](#cicd)
* [Recommended Extensions](#recommended-extensions)
* Installation
  * [Docker](doc/readme/Docker.md)
  * [Microsoft Windows](doc/readme/Windows.md)
  * [Ubuntu](doc/readme/Ubuntu.md)

## Environment

Set the following environment variables to control and configure the behaviour of the application:

|Name|Default|Description|
|-|:-:|-|
|`LOG_IGNORED_LEXEMES`|`true`|When `true`, logs all of the ignored lexemes found with Flex at DEBUGGING level. To remove those logs from the console output set it to `false`.|
|`LOGGING_LEVEL`|`INFORMATION`|The minimum level to log in the console output. From lower to higher, the available levels are: `ALL`, `DEBUGGING`, `INFORMATION`, `WARNING`, `ERROR` and `CRITICAL`.|

## CI/CD

To trigger an automatic integration on every push or PR (_Pull Request_), you must activate _GitHub Actions_ in the _Settings_ tab. Use the following configuration:

|Key|Value|
|-|-|
|Actions permissions|Allow all actions and reusable workflows|
|Artifact and log retention|`30 days`|
|Fork pull request workflows from outside collaborators|Require approval for all outside collaborators|
|Workflow permissions|Read repository contents and packages permissions|
|Allow GitHub Actions to create and approve pull requests|`false`|

After integration is done, change the badges of this `README.md` to point to the new repository.

## Recommended Extensions

* [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
* [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)
* [Yash](https://marketplace.visualstudio.com/items?itemName=daohong-emilio.yash)
