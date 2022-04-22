function loadPayloadTables() {
    console.log("Loading payloads...");

    const xhttp = new XMLHttpRequest();
    xhttp.open("GET", "http://localhost/data.json?rand=" + Date.now(), false); // prove endpoint to return json
    xhttp.send(null);

    var json = xhttp.responseText;
    const obj = JSON.parse(json);
    const pTables = ["tableStarred", "tableLinux", "tableMac", "tableWindows"];
    const pTypes = ["Starred", "Linux", "Mac", "Windows"];
    const pEditOs = ["editStarred", "editLinux", "editMac", "editWindows"]

    // append to each table
    for (let i = 0; i < 4; i++) {
        for (let j = 0; j < Object.keys(obj).length; j++) {
            try {
                var payload = obj[j];
                if (payload.pOS.toUpperCase() == pTypes[i].toUpperCase()) {
                    const table = document.createElement("tr");
                    const payloadName = document.createElement("td"); payloadName.innerText = payload.pName; payloadName.classList.add("payloadname");
                    const payloadEdit = document.createElement("td"); payloadEdit.innerHTML = "<button class=\"edit\" onclick=\"editPayload(\'/"+pTypes[i]+"/"+ payload.pCategory + "/" + payload.pName + "\',\'" + pEditOs[i] + "')\">Edit</button>";
                    const payloadRun = document.createElement("td"); payloadRun.innerHTML = "<button class=\"run\" onclick=\"runPayload('"+pEditOs[i]+"',\'/"+pTypes[i]+"/" + payload.pCategory + "/" + payload.pName + "\')\">Run</button>";
                    
                    table.appendChild(payloadName);
                    table.appendChild(payloadEdit);
                    table.appendChild(payloadRun);

                    document.getElementById(pTables[i]).appendChild(table);
                }
            }
            catch (error) {
                break;
            }
        }
    }
}

// run payload with path on internal server
function runPayload(payloadEdit, payloadPath) {
    closeEdit();

    var editArea = document.getElementById(payloadEdit);
    editArea.innerHTML = "<p id=\"statusText\">Running: <b>" + payloadPath + "</b></p>";
    
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.open("GET", "runpayload.php?path="+payloadPath, false); // false for synchronous request
    xmlHttp.send(null);
    return xmlHttp.responseText;
}

function editPayload(payloadPath, payloadEdit) {
    closeEdit();
    var editArea = document.getElementById(payloadEdit);
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.open("GET", "getpayload.php?path="+payloadPath, false); // false for synchronous request
    xmlHttp.send(null);

    editArea.innerHTML = "<p id=\"statusText\">Editing: <b>" + payloadPath + "</b></p>\
<div class=\"row\"> \
<div class=\"input-field col s12\"> \
<textarea id=\"textarea1\" rows = 10 class=\"materialize-textarea\">"+atob(xmlHttp.responseText)+"\n</textarea> \
</div> \
<button class=\"run\" onclick=\"runLive(\'"+payloadEdit+"\')\">Run Live</button> \
<button class=\"edit\" onclick=\"savePayload('"+payloadEdit+"','"+payloadPath+"')\">Save</button> \
</div> \
";
}

function closeEdit(payloadEdit) {
    document.getElementById("editStarred").innerHTML = "";
    document.getElementById("editLinux").innerHTML = "";
    document.getElementById("editMac").innerHTML = "";
    document.getElementById("editWindows").innerHTML = "";
}

// take encode

function runLive(payloadEdit) {
    document.getElementById("statusText").innerHTML="<p><b>Running Live!</b></p>";
    var editArea = document.getElementById(textarea1);

    var xmlHttp = new XMLHttpRequest();
    xmlHttp.open("POST", "runlive.php");
    xmlHttp.setRequestHeader("Content-Type", "plain/text");
    xmlHttp.send(btoa(document.getElementById("textarea1").value));

}

// merge into single function? ugh
function savePayload(payloadEdit, payloadPath) {
    document.getElementById("statusText").innerHTML="<p>Saved: <b>"+payloadPath+"</b></p>";
    var editArea = document.getElementById(textarea1);

    // encode base64 and send 
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.open("POST", "savepayload.php");
    xmlHttp.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    xmlHttp.send("path="+payloadPath+"&content="+btoa(textarea1.value));
}