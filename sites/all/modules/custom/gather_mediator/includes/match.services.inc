<?php
function gather_mediator_match_services_resource() {
    $api = array(
        'match_team' => array(
            'actions' => array(
                'get_team' => array(
                    'help' => 'Match team players.',
                    'file' => array(
                        'type' => 'inc',
                        'module' => 'gather_mediator',
                        'name' => 'includes/match.services',
                    ),
                    'callback' => 'match_team_services_resource_retrieve',
                    'access callback' => 'match_team_services_resource_access',
                    'access arguments' => array('create'),
                    'access arguments append' => FALSE,
                    'args' => array(
                        array(
                            'name' => 'club_ids',
                            'type' => 'array',
                            'description' => 'The dfdgd user object',
                            'source' => 'data',
                            'optional' => FALSE,
                        ),
                    ),

                ),
                'create_team' => array(
                    'help' => 'Match Node Create',
                    'file' => array(
                        'type' => 'inc',
                        'module' => 'gather_mediator',
                        'name' => 'includes/match.services',
                    ),
                    'callback' => 'gather_mediator_resource_create',
                    'access callback' => 'match_team_services_resource_access',
                    'access arguments' => array('create'),
                    'access arguments append' => FALSE,
                    'args' => array(
                        array(
                            'name' => 'node',
                            'type' => 'array',
                            'description' => 'Iterate node object',
                            'source' => 'data',
                            'optional' => FALSE,
                        ),
                    ),

                ),
                'club_listing' => array(
                    'help' => 'Match Node Create',
                    'file' => array(
                        'type' => 'inc',
                        'module' => 'gather_mediator',
                        'name' => 'includes/match.services',
                    ),
                    'callback' => 'gather_mediator_club_listing_resource_create',
                    'access callback' => 'match_team_services_resource_access',
                    'access arguments' => array('create'),
                    'access arguments append' => FALSE,
                    'args' => array(
                        array(
                            'name' => 'node',
                            'type' => 'array',
                            'description' => 'Iterate node object',
                            'source' => 'data',
                            'optional' => FALSE,
                        ),
                    ),

                ),
                'live_match_team' => array(
                    'help' => 'Match Node Create',
                    'file' => array(
                        'type' => 'inc',
                        'module' => 'gather_mediator',
                        'name' => 'includes/match.services',
                    ),
                    'callback' => 'gather_mediator_live_match_team_resource_view',
                    'access callback' => 'match_team_services_resource_access',
                    'access arguments' => array('create'),
                    'access arguments append' => FALSE,
                    'args' => array(
                        array(
                            'name' => 'match_id',
                            'type' => 'array',
                            'description' => 'Iterate node object',
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

function match_team_services_resource_retrieve($club_ids) {
   // echo "nailed"; var_dump($club_ids); die();
    if(empty($club_ids['home_club_id']) || !isset($club_ids['home_club_id'])){
        $error = "Please provide home Club Id's";
        return services_error(t("Club Id Error"),406,array("club_id_error"=>$error));
    }
    if(empty($club_ids['opponent_club_id']) || !isset($club_ids['opponent_club_id'])){
        $error = "Please provide opponent team id";
        return services_error(t("Club Id Error"),406,array("club_id_error"=>$error));
    }
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'team_player')
        ->fieldCondition('field_first_team', 'target_id ', $club_ids['home_club_id'], '=');

    $home_team_result = $query->execute();
    $i = 0;
    $data=array();
    foreach($home_team_result['node'] as $key=>$teamId) {
        $nid[]=$key;

    }
    $team_details = node_load_multiple($nid,array('type'=>"team_player"));
    $sql = "SELECT title FROM {node} WHERE nid =".$club_ids['home_club_id'];
   // $result = db_query($sql, array($club_ids['home_club_id']));
    $result = db_query($sql)->fetchField();

    $data['home_club']['club_id'] = $club_ids['home_club_id'];
    $data['home_club']['club_name'] = $result;

    foreach($team_details as $key=>$team_detail):
        $data['home_club']['home_team'][$i]['team_id'] =  $key;
        $data['home_club']['home_team'][$i]['field_team_player_image'] ='';
        if(!empty($team_detail->field_team_player_image)) {
            $data['home_club']['home_team'][$i]['field_team_player_image'] = file_create_url($team_detail->field_team_player_image['und'][0]['uri']);
        }
        $data['home_club']['home_team'][$i]['team_player'] =  $team_detail->title;
        $data['home_club']['home_team'][$i]['field_team_jersey_number'] =  $team_detail->field_team_jersey_number['und'][0]['value'];

        //$data->home_team->team_player =  $team_details->title;


        $i++;
    endforeach;


    $i = 0;
    $query2 = new EntityFieldQuery();
    $query2->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'team_player')
        ->fieldCondition('field_first_team', 'target_id ', $club_ids['opponent_club_id'], '=');
    $opp_team_result = $query2->execute();

    foreach($opp_team_result['node'] as $key=>$teamId) {


        $nid2[]=$key;

    }
    $team_details = node_load_multiple($nid2,array('type'=>"team_player"));
    $sql = 'SELECT title FROM {node} WHERE nid ='.$club_ids['opponent_club_id'];
    $result = db_query($sql)->fetchField();

    $data['opponent_club']['club_id'] = $club_ids['opponent_club_id'];
    $data['opponent_club']['club_name'] = $result;
    foreach($team_details as $key=>$team_detail):
        $data['opponent_club']['opponent_team'][$i]['team_id'] =  $key;
        $data['opponent_club']['opponent_team'][$i]['field_team_player_image'] = '';
        if(!empty($team_detail->field_team_player_image)) {
            $data['opponent_club']['opponent_team'][$i]['field_team_player_image'] = file_create_url($team_detail->field_team_player_image['und'][0]['uri']);
        }
        $data['opponent_club']['opponent_team'][$i]['team_player'] =  $team_detail->title;
        $data['opponent_club']['opponent_team'][$i]['field_team_jersey_number'] =  $team_detail->field_team_jersey_number['und'][0]['value'];
        //$data->home_team->team_player =  $team_details->title;
        $i++;
    endforeach;



    return $data;

}


/**
 * Creates a new node based on submitted values.
 *
 * Note that this function uses drupal_form_submit() to create new nodes,
 * which may require very specific formatting. The full implications of this
 * are beyond the scope of this comment block. The Googles are your friend.
 *
 * @param $node
 *   Array representing the attributes a node edit form would submit.
 * @return
 *   An associative array contained the new node's nid and, if applicable,
 *   the fully qualified URI to this resource.
 *
 * @see drupal_form_submit()
 */
function gather_mediator_resource_create($node) {

    global $user;
    // Adds backwards compatability with regression fixed in #1083242
    $node = _services_arg_value($node, 'node');
    if (!isset($node['name'])) {
        // Assign username to the node from $user created at auth step.
        if (isset($user->name)) {
            $node['name'] = $user->name;
        }
    }
    if(!isset($node['language'])) {
        $node['language'] = LANGUAGE_NONE;
    }
    // Validate the node. If there is validation error Exception will be thrown
    // so code below won't be executed.
    _node_resource_validate_type($node);

    // Load the required includes for drupal_form_submit
    module_load_include('inc', 'node', 'node.pages');

    $node_type = $node['type'];

    //print_r($node); die();

    $match = new stdClass();  // Create a new node object
    $match->type = $node['type'];  // Content type
    $match->language = $node['language'];  // Or e.g. 'en' if locale is enabled
    node_object_prepare($match);  //Set some default values

    $match->title = $node['title'];
    $match->status = 1;   // (1 or 0): published or unpublished
    $match->promote = 0;  // (1 or 0): promoted to front page or not
    $match->sticky = 0;  // (1 or 0): sticky at top of lists or not
    $match->comment = 1;  // 2 = comments open, 1 = comments closed, 0 = comments hidden
// Add author of the node
    $match->uid = $user->uid;

// Save the node
    node_save($match);



    return $match;
}



function gather_mediator_live_match_team_resource_view($match_id){
    global $user;
    $match = node_load($match_id['match_id']);

    $club_target_id = $match->field_club_detail['und'][0]['value'];
    $club_details = entity_load('field_collection_item', array($club_target_id));
    $home_target_id = $match->field_home_team_detail['und'];
    $home_club_details = entity_load('field_collection_item', $home_target_id);
    $opp_target_id = $match->field_opponent_team_detail['und'];
   $opp_club_details = entity_load('field_collection_item', $opp_target_id);
   // print_r($club_details);die;
    $data = array();
    $i = 0;
    $data['home_club']['club_id'] = $club_details[$club_target_id]->field_home_club_id['und'][0]['value'];
    $data['home_club']['club_name'] = $club_details[$club_target_id]->field_home_club_name['und'][0]['value'];
    foreach($home_club_details as $key=>$team_detail):
        //echo '<pre>';print_r($team_detail);die;
        $data['home_club']['home_team'][$i]['team_id'] =  $team_detail->field_tid['und'][0]['value'];
        $data['home_club']['home_team'][$i]['team_player'] =  $team_detail->field_tname['und'][0]['value'];
        $data['home_club']['home_team'][$i]['field_team_jersey_number'] =  $team_detail->field_tnumber['und'][0]['value'];
        $i++;

    endforeach;
    $i = 0;
   // $data['opponent_club']['club_id'] = $club_ids['opponent_club_id'];
   // $data['opponent_club']['club_name'] = $result;

    $data['opponent_club']['club_id'] = $club_details[$club_target_id]->field_opponent_club_id['und'][0]['value'];
    $data['opponent_club']['club_name'] = $club_details[$club_target_id]->field_opponent_club_name['und'][0]['value'];
    foreach($opp_club_details as $key=>$team_detail):

        $data['opponent_club']['opponent_team'][$i]['team_id'] =  $team_detail->field_tid['und'][0]['value'];;
        $data['opponent_club']['opponent_team'][$i]['team_player'] =  $team_detail->field_tname['und'][0]['value'];
        $data['opponent_club']['opponent_team'][$i]['field_team_jersey_number'] =  $team_detail->field_tnumber['und'][0]['value'];
        $i++;

    endforeach;


    return $data;

}




function match_team_services_resource_access($op = 'view', $args = array()) {
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
