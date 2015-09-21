<?php
	require 'vendor/autoload.php';
 
	use Parse\ParseClient;
 
	ParseClient::initialize('DgkajZ46p0LCwrwpfUFmbCeGoIizgVbo6z4LUEhP', 'E1FoL8819zAOM7DEWyzIqpscEsFXtpMPDMW06V2v', 'igNDR8tbxyRhEMQVIcJicITXGusk5sPCy5cYDDdz');
	use Parse\ParseObject;
 	use Parse\ParseQuery;

 	$query = new ParseQuery("MachineMetadata");
	$query->equalTo("deviceName", "Temp Sensor");
	$results = $query->find();
	echo $results[0]->get("isOn");
?>