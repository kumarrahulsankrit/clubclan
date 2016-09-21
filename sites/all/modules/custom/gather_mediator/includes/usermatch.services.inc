<?php
function gather_mediator_usermatch_services_resource() {
    $api = array(
        'user_match_likes' => array(
            'operations' => array(
                'retrieve' => array(
                    'help' => 'Retrieve a matches of a user',
                    'callback' => 'gather_mediator_usermatch_resource_retrieve',
                    'file' => array('type' => 'inc', 'module' => 'gather_mediator', 'name' => 'includes/usermatch.services'),
                    'access callback' => 'gather_mediator_usermatch_resource_access',
                    'access arguments' => array('view'),
                    'access arguments append' => TRUE,
                    'args' => array(
                        array(
                            'name' => 'uid',
                            'type' => 'int',
                            'description' => 'The uid of the user to retrieve.',
                            'source' => array('path' => 0),
                            'optional' => FALSE,
                        ),
                    ),
                ),

            ),
        ),

    );


    return $api;
}
function gather_mediator_home_feed_url_services_resource() {
    $api = array(
        'home_feed_url' => array(
            'operations' => array(
                'index' => array(
                    'help' => 'Home Feed link',
                    'callback' => 'gather_mediator_home_feed_resource_retrieve',
                    'file' => array('type' => 'inc', 'module' => 'gather_mediator', 'name' => 'includes/usermatch.services'),
                    'access callback' => 'gather_mediator_usermatch_resource_access',
                    'access arguments' => array('view'),
                    'access arguments append' => TRUE,
                    'args' => array(
                        array(
                            'name' => 'uid',
                            'type' => 'int',
                            'description' => 'The uid of the user to retrieve.',
                            'source' => array('path' => 0),
                            'optional' => TRUE,
                        ),
                    ),
                ),

            ),
        ),

    );


    return $api;
}



function gather_mediator_usermatch_resource_retrieve($uid){

    $user = user_load($uid);

    $query = new EntityFieldQuery;
    $result = $query->entityCondition('entity_type', 'node')
        ->propertyCondition('status', 1)
        ->propertyCondition('type', 'matches')
        ->propertyCondition('uid', $uid)
        ->execute();

    $nids = array();
    foreach($result['node'] as $nid => $nodes){
        $node = node_load($nid);
        if(isset($node->field_match_likes['und'][0]['value'])){
            $user_like[] = $node->field_match_likes['und'][0]['value'];

        }
    }

    $user_likes = array_sum($user_like);
    $data['user_likes'] = $user_likes;
    return $data;

}



function gather_mediator_home_feed_resource_retrieve($uid){
    global $base_url;
    $home_local_url = "$base_url/ccs/news";
    $home_feed_url = variable_get('home_feed_url', 'value');
    if(empty($home_feed_url)){
        $data['home_feed_url'] = $home_local_url;
    } else{
        $data['home_feed_url'] = variable_get('home_feed_url', 'value');
    }
    return $data;

}


function gather_mediator_usermatch_resource_access($op = 'view', $args = array()) {
    // Adds backwards compatability with regression fixed in #1083242


    if (isset($args[0])) {
        $args[0] = _services_access_value($args[0], array('account', 'data'));
    }

    // Check if the user exists if appropriate.
/*    if ($op != 'create' && $op != 'register' ) {
        $account = user_load($args[0]);
        if (!$account) {
            return services_error(t('There is no user with ID @uid.', array('@uid' => $args[0])), 406);
        }
    }
*/
    global $user;
    switch ($op) {
        case 'view':
       //     return user_view_access($account);
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
