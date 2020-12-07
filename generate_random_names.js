const fs = require("fs");
const crypto = require("crypto")
let raw_file = fs.readFileSync("tilfældigenavne.txt").toString();
let navne_liste = raw_file.split("\r\n").filter((v) => v.length > 0).map((v) => v.trim());
let vagt_liste = ["nat", "dag", "aften"];
let dag_list = ["mandag", "tirsdag", "onsdag", "torsdag", "fredag", "lørdag", "søndag"];
let uuid_counter = 0;

function tilfældig(liste) {
    return liste[crypto.randomInt(0, liste.length - 1)];
}
uuid_counter = 0;
let file_buffer = "";
for (let i = 0; i < 400; i++) {
    file_buffer += `${tilfældig(navne_liste)}, ${tilfældig(dag_list)}, ${tilfældig(vagt_liste)}, ${uuid_counter++}\n`;
}

fs.writeFileSync("input.csv", file_buffer);