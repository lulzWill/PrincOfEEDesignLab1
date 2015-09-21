<?php
	require 'vendor/autoload.php';
 
	use Parse\ParseClient;
 
	ParseClient::initialize('DgkajZ46p0LCwrwpfUFmbCeGoIizgVbo6z4LUEhP', 'E1FoL8819zAOM7DEWyzIqpscEsFXtpMPDMW06V2v', 'igNDR8tbxyRhEMQVIcJicITXGusk5sPCy5cYDDdz');
	use Parse\ParseObject;
 
	$testObject = ParseObject::create("TestObject");
	$testObject->set("foo", "name");
	$testObject->save();
?>