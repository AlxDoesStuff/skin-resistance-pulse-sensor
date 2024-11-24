const TABLE_DIRECTORY = "./data/"; //Ordner, in dem die Tabelle gespeichert wird
Deno.serve({ port: 8000, hostname: "0.0.0.0" }, handler); //Server auf 8000 hosten
//Request Handler
async function handler(_req: Request){
  const requestURL = _req.url; //Request-URL speichern
  const URLSplit = requestURL.split("/"); //Request-URL in bestandteile trennen
  //Unterscheide zwischen Verbindungsankündigung, Daten und Müll
  if(URLSplit[3]){
    if(URLSplit[3] == "connected" && URLSplit.length == 6){
      const connectIDParsed = parseInt(URLSplit[5]); //Die ID des verbundenen ESPs 
      //Datum
      const realTime = new Date();
      const DD = ("0" + realTime.getDate()).slice(-2);
      const MM = ("0" + (realTime.getMonth() + 1)).slice(-2);
      const YYYY = realTime.getFullYear();
      const hh = realTime.getHours();
      const mm = realTime.getMinutes();
      const ss = realTime.getSeconds();
      //Tabelle erstellen mit Name: ID + Datum und Uhrzeit und Erste Zeile: Zeit, Puls und Widerstand
      const newTableName = TABLE_DIRECTORY + connectIDParsed + "--"+DD+"-"+MM+"-"+YYYY+"-"+hh+"-"+mm+"-"+ss+".csv" //Name der neuen Tabelle
      await Deno.writeTextFile(newTableName, "Zeit,Puls,Widerstand\n"); //Neue Tabelle erstellen
      await Deno.writeTextFile(TABLE_DIRECTORY + connectIDParsed.toString(), newTableName); //Die neue Tabelle für das ID eintragen
    }
    else if(URLSplit[3] == "data" && URLSplit.length == 8){
      //Daten aus der URL entlesen
      const dataTimeParsed = parseInt(URLSplit[6]); //Die Zeit, seitdem der Verbundene ESP läuft in ms
      const dataIDParsed = parseInt(URLSplit[7]); //Die ID des ESPs, der die Daten sendet
      const pulseParsed = parseInt(URLSplit[4]);  //Gemessener Puls
      const skinResistanceParsed = parseInt(URLSplit[5]); //Gemessener Hautwiderstand
      //Tabelle für das ID finden
      const tableNameRaw = await Deno.readFile(TABLE_DIRECTORY + dataIDParsed); //Tabellenregister lesen als Bytes
      const decoder = new TextDecoder("utf-8"); //UTF-8 byte decoder
      const tableName = decoder.decode(tableNameRaw); //Decodierter Tabellenname
      //Daten in die entsprechende Tabelle eintragen
      await Deno.writeTextFile(tableName, dataTimeParsed + "," + pulseParsed + "," + skinResistanceParsed +"\n", { append: true });
    }
    else{  
      console.log("Incorrect URL: "+requestURL); //Falsche URL-formate zur Konsole ausgeben
      return new Response("404"); //404 antworten
    }
    
  }  return new Response("OK"); //Auf alle korrekten Requests OK antworten 
}

