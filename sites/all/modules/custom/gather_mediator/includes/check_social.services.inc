<?php
function gather_mediator_services_resource() {
  $api = array(
  'check_social' => array(
    'operations' => array(
      'create' => array(
        'help' => 'check social.',
        'file' => array(
          'type' => 'inc',
          'module' => 'gather_mediator',
          'name' => 'includes/check_social.services',
        ),
        'callback' => 'check_social_services_resource_retrieve',
          'access callback' => 'check_social_services_resource_access',
          'access arguments' => array('create'),
          'access arguments append' => FALSE,
          'args' => array(
              array(
                  'name' => 'social',
                  'type' => 'array',
                  'description' => 'The social user object',
                  'source' => 'data',
                  'optional' => FALSE,
              ),
          ),

        ),
      ),
        ),

  );


  return $api;
}

/**
 * [housing_schemes_services_resource_retrieve] definition.
 * Returns the information about a housing scheme.
 * @param $nid
 *   The nid of the housing_scheme node.
 * @return array
 *  The node processed information array.
 */
function check_social_services_resource_retrieve($social) {

    if(empty($social['socialId']) || !isset($social['socialId'])){
        $error = "Please provide social Id";
        return services_error(t("social Error"),406,array("social_error"=>$error));
    }
    if(empty($social['social_source']) || !isset($social['social_source'])){
        $error = "Please provide social source";
        return services_error(t("social Error"),406,array("social_error"=>$error));
    }
    $social_source_array = array('facebook',"twitter");
    if(!in_array($social['social_source'],$social_source_array)){
        $error = "Please provide social source facebook or twitter";
        return services_error(t("social Error"),406,array("social_error"=>$error));
    }
    $data = new stdClass();
    if(!empty($social['socialId'])){
        $query = new EntityFieldQuery();
        $query->entityCondition('entity_type', 'user');
        if($social['social_source'] == "facebook") {

            $query->fieldCondition('field_fb_userid', 'value', $social['socialId'], '=');
        }
        if($social['social_source'] == "twitter") {

            $query->fieldCondition('field_twitter_user_id', 'value', $social['socialId'], '=');
        }
        $results = $query->execute();

        if (isset($results['user'])) {
            $user_uid = array_keys($results['user']);
            $user_details = entity_load('user', $user_uid);
            $uidFT = $user_uid[0];
            $data->uid =  $uidFT;
            $data->username =  $user_details[$uidFT]->name;
            $data->password = $social['socialId'];
            $data->socialcheck = 1;

            return $data;
        }
        else{
            $data->socialcheck = 0;
            return $data;
        }
    }

   // return (object)array("socialcheck"=>0);
}

function check_social_services_resource_access($op = 'view', $args = array()) {
    // Adds backwards compatability with regression fixed in #1083242
    if (isset($args[0])) {
        $args[0] = _services_access_value($args[0], array('account', 'data'));
    }

    // Check if the user exists if appropriate.
    if ($op != 'create' && $op != 'register' ) {
        $account = user_load($args[0]);
        if (!$account) {
            return services_error(t('There is no user with ID @uid.', array('@uid' => $args[0])), 406);
        }
    }

    global $user;
    switch ($op) {
        case 'view':
            return user_view_access($account);
        case 'update':
            return ($user->uid == $account->uid || user_access('administer users'));
        case 'create':
        case 'register':
            if (!$user->uid && variable_get('user_register', USER_REGISTER_VISITORS_ADMINISTRATIVE_APPROVAL) != USER_REGISTER_ADMINISTRATORS_ONLY) {
                return TRUE;
            }
            else {
                return user_access('administer users');
            }
        case 'password_reset':
            return TRUE;
        case 'delete':
        case 'cancel':
        case 'resend_welcome_email':
            return user_access('administer users');
    }
}