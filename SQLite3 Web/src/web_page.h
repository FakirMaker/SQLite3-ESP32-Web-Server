#ifndef WEB_PAGE_H
#define WEB_PAGE_H

const char web_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<title>SQL EXAMPLE</title>
<meta http-equiv="Content-Type" content="text/html;charset=UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
:root {
  --main-bg-color: #4CAF50;
}
body{
    font-family:Arial
    }
.header{padding:10px;
    text-align:left;
    background:var(--main-bg-color);
    color:#fff;
    font-size:20px
    }
table,th,td{
    font-family:arial,sans-serif;
    border:1px solid #000;
    border-collapse:collapse
    }
th,td{
    padding:2px
    }
th{
    text-align:left;
    background-color:var(--main-bg-color);
    color:#fff
    }
.tab{
    overflow:hidden;
    border:1px solid #ccc;background-color:#f1f1f1
    }
.tab button{
    background-color:inherit;
    float:left;
    border:none;
    outline:none;
    cursor:pointer;
    padding:14px 16px;
    transition:0.3s;
    font-size:17px
    }
.tab button:hover{
    background-color:#ddd
    }
.tab button.active{
    background-color:#ccc
    }
.tabcontent{
    display:none;
    padding:6px 12px;
    border:1px solid #ccc;
    border-top:none
    }
div.scroll_stok{
    height:300px;
    overflow-x:hidden;
    overflow-y:auto
    }
div.scroll_stok_mov
{
    height:500px;
    overflow-x:hidden;
    overflow-y:auto
    }
footer{
    text-align:left;
    background-color:var(--main-bg-color);
    color:#fff;
    display:block
    }
</style>
<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js"></script>
<script>
$(document).ready(function(){
   
   $('#table').load("/show");

});
function newRecordTable(){
     $('#table').load("/newRecordTable");
}
function insert(){
    $.post('/insert',   // url
       { personal_id: $("#pid").val(),name: $("#pname").val(),surname: $("#psname").val(),start_date: $("#sdate").val(),salary: $("#salary").val() }, // data to be submit
       function(data, status, jqXHR) {// success callback
                $('#table').load("/show");
        })
} 
function del(param){
        $.post('/delete',   // url
       { personal_id: param }, // data to be submit
       function(data, status, jqXHR) {// success callback
                $('#table').load("/show");
        })
}
function cancel(){
    $('#table').load("/show");
}
</script>
</head>
<body>
<div class="header">
<h2>SQLite Server</h2>
</div>
<div class="tab">
  <button class="tablinks" id="insertBut" onclick="newRecordTable()">Insert New Record</button>
</div>
<br><br>
<div id="table"></div>
</body>
</html>    
)rawliteral";

#endif