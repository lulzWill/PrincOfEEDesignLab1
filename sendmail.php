<?php
	// this line loads the library 
	require('twilio-php/Services/Twilio.php'); 
	 
	$account_sid = 'AC023f49fa59edf696b6b5ae769024f2e3'; 
	$auth_token = '6138e5f23536f974652edbc4d2b4c8bc'; 
	$client = new Services_Twilio($account_sid, $auth_token); 
	 
	$client->account->messages->create(array(  
		'To' => "16307449777",
		'From' => "+13317033057", 
		'Body' => $_GET["body"],    
	));
?>