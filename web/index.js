function loadPayloadTables() {
    console.log("Loading payloads...");

    const xhttp = new XMLHttpRequest();
    xhttp.open("GET", "http://localhost/data.json?rand="+Date.now(), false); // prove endpoint to return json
    xhttp.send(null);

    var json = xhttp.responseText;
    const obj = JSON.parse(json);
    const pTables = ["pStarred", "pLinux","pMac","pWindows"];
    const pTypes = ["STARRED", "LINUX","MAC","WINDOWS"];
    const pEditOs = ["editStarred","editLinux","editMac","editWindows"]

    // append to each table
    for (let i = 0; i < 4; i++) { 
        for (let j = 0; j < Object.keys(obj).length; j++) {
            try {
                var payload = obj[j];
                if (payload.pOS.toUpperCase() == pTypes[i]) {
                    const table = document.createElement("tr");
                    const payloadName = document.createElement("td");  payloadName.innerText = payload.pName;
                    const payloadCategory = document.createElement("td"); payloadCategory.innerText = payload.pCategory;
                    const payloadEdit = document.createElement("td"); payloadEdit.innerHTML = "<a class=\"waves-effect waves-light teal darken-2 btn\" onclick=\"editPayload(\'/"+payload.pCategory+"/"+payload.pName+"/\',\'"+pEditOs[i]+"\')\">Edit</a>";
                    const payloadRun = document.createElement("td"); payloadRun.innerHTML = "<a class=\"waves-effect waves-light red darken-2 btn\" onclick=\"runPayload(\'/"+payload.pCategory+"/"+payload.pName+"/\')\">RUN PAYLOAD</a>";
                   
                    table.appendChild(payloadName);
                    table.appendChild(payloadCategory);
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

function savePayload() {

}

// run payload with path on internal server
function runPayload(payloadPath) {
    closeEdit();
    console.log(payloadPath);
}

function editPayload(payloadPath,payloadEdit) {
    closeEdit();
    var editArea = document.getElementById(payloadEdit);
    editArea.innerHTML = "<hr><p><i class=\"material-icons right\" style=\"cursor: pointer;\" onclick=\"closeEdit()\">close</i> Editing: <b>"+payloadPath+"</b></p>\
    <div class=\"row\"> \
    <div class=\"input-field col s12\"> \
    <i class=\"material-icons prefix\">code</i> \
    <textarea id=\"textarea1\" class=\"materialize-textarea\">asdfasdfa\nasdfasfda\n</textarea> \
    <label for=\"textarea1\">Payload Content</label> \
    </div> \
    <a class=\"waves-effect waves-light btn red darken-2\" onclick=\"runLive()\"><i class=\"material-icons right\">flash_on</i>Run Live</a> \
    <a class=\"waves-effect waves-light btn teal darken-2\"><i class=\"material-icons right\">save</i>Save</a> \
    </div> \
    ";
    M.updateTextFields();
    M.textareaAutoResize(document.getElementById('textarea1'));
}

function closeEdit(payloadEdit) {
    document.getElementById("editStarred").innerHTML ="";
    document.getElementById("editLinux").innerHTML ="";
    document.getElementById("editMac").innerHTML ="";
    document.getElementById("editWindows").innerHTML ="";
}

function runLive() {
    console.log("running live");
}

function savePayload() {

}