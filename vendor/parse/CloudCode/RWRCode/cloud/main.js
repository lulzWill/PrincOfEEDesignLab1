
// Use Parse.Cloud.define to define as many cloud functions as you want.
// For example:
Parse.Cloud.define("hello", function(request, response) {
  response.success("Hello world!");
});

Parse.Cloud.define("checkUser", function(request, response) {
  var user = request.user;
  console.log(user);
  response.success(user.get("email"));
});
Parse.Cloud.define("modifyUser", function(request, response) {
  if (!request.user) {
	  console.log(Parse.User.current());
	response.error("oh noooooooo")
	return;
  }
  // The user making this request is available in request.user
  // Make sure to first check if this user is authorized to perform this change.
  // One way of doing so is to query an Admin role and check if the user belongs to that Role.
  // Replace !authorized with whatever check you decide to implement.
  if (request.user.get("position") != "admin") {
	response.error("Not an Admin.")
	return;    
  }

  // The rest of the function operates on the assumption that request.user is *authorized*

  Parse.Cloud.useMasterKey();
  // Query for the user to be modified by username
  // The username is passed to the Cloud Function in a 
  // key named "username". You can search by email or
  // user id instead depending on your use case.
  var query = new Parse.Query(Parse.User);
  query.equalTo("email", request.params.email);
  // Get the first user which matches the above constraints.
  query.first({
	success: function(anotherUser) {
	  // Successfully retrieved the user.
	  // Modify any parameters as you see fit.
	  // You can use request.params to pass specific
	  // keys and values you might want to change about
	  // this user.
	  anotherUser.set("firstname", request.params.firstname);
	  anotherUser.set("lastname", request.params.lastname);
	  anotherUser.set("dateOfBirth", request.params.dateOfBirth);
	  anotherUser.set("position", request.params.position);
	  anotherUser.set("sex", request.params.sex);
	  
	  // Save the user.
	  anotherUser.save(null, {
		success: function(anotherUser) {
		  // The user was saved successfully.
		  response.success("COMPLETE")
		},
		error: function(object, error) {
		  // The save failed.
		  // error is a Parse.Error with an error code and description.
		}
	  });
	},
	error: function(error) {
	  response.error("Could not find user.");
	}
  });
});
