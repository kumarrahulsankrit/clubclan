<?php
function gather_mediator_match_data_services_resource() {
    $api = array(
        'match_data' => array(
            'operations' => array(
                'retrieve' => array(
                    'help' => 'Retrieve a match',
                    'file' => array('type' => 'inc', 'module' => 'gather_mediator', 'name' => 'includes/match_data.services'),
                    'callback' => 'gather_mediator_match_data_resource_retrieve',
                    'args' => array(
                        array(
                            'name' => 'nid',
                            'optional' => FALSE,
                            'source' => array('path' => 0),
                            'type' => 'int',
                            'description' => 'The nid of the node to retrieve',
                        ),
                    ),
                    'access callback' => 'gather_mediator_match_data_resource_access',
                    'access arguments' => array('view'),
                    'access arguments append' => TRUE,
                ),
                'update' => array(
                    'help' => 'Update a Match.',
                    'file' => array('type' => 'inc', 'module' => 'gather_mediator', 'name' => 'includes/match_data.services'),
                    'callback' => 'gather_mediator_match_data_resource_update',
                    'args' => array(
                        array(
                            'name' => 'nid',
                            'optional' => FALSE,
                            'source' => array('path' => 0),
                            'type' => 'int',
                            'description' => 'The nid of the node to update',
                        ),
                        array(
                            'name' => 'node',
                            'optional' => FALSE,
                            'source' => 'data',
                            'description' => 'The node data to update',
                            'type' => 'array',
                        ),
                    ),
                    'access callback' => 'gather_mediator_match_data_resource_access',
                    'access arguments' => array('update'),
                    'access arguments append' => TRUE,
                ),
                'create' => array(
                    'help' => 'Club Live Chat Create',
                    'file' => array(
                        'type' => 'inc',
                        'module' => 'gather_mediator',
                        'name' => 'includes/match_data.services',
                    ),
                    'callback' => 'gather_mediator_clubs_matches_live_chat_create',
                    'access callback' => 'gather_mediator_match_data_resource_access',
                    'access arguments' => array('create'),
                    'access arguments append' => FALSE,
                    'args' => array(
                        array(
                            'name' => 'node',
                            'type' => 'array',
                            'description' => 'create the live match.',
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

function gather_mediator_clubs_matches_live_chat_create($node){
    //echo '<pre>';print_r($node);die;
    if(!empty($node)) {
        $entity = entity_create('example_task', array('type' => 'live_match_chat'));
        $entity->title = $node['title'];
        $entity->description = $node['description'];
        $entity->nid = $node['nid'];
        $entity->match_thumb = $node['match_thumb'];
        $entity->save();
        return $entity;
    }
    return false;
    //Not all entities hav

}




function gather_mediator_match_data_resource_retrieve($nid){

    global $user;
    $match = node_load($nid);
    $user_load = user_load_by_name($match->name);
    $data['user_id'] = $user_load->uid;
    $data['first_name'] = $user_load->field_first_name['und'][0]['value'];
    $data['last_name'] = $user_load->field_last_name['und'][0]['value'];
    $data['user_image'] = file_create_url($user_load->field_user_image['und'][0]['uri']);
    $data["match_id"] = $match->nid;

    $data["match_name"] = $match->title;
    $data['match_start_date'] = $match->field_match_date['und'][0]['value'];
    $data['match_end_date'] = $match->field_match_date['und'][0]['value2'];
    $data["match_likes"] = $match->field_match_likes['und'][0]['value'];
    $target_id = $match->field_club_detail['und'][0]['value'];
    $club_details = entity_load('field_collection_item', array($target_id));
    $home_club = node_load($club_details["$target_id"]->field_home_club_id['und'][0]['value']);
    $opp_club = node_load($club_details["$target_id"]->field_opponent_club_id['und'][0]['value']);
    $data["home_team_id"] = $club_details["$target_id"]->field_home_club_id['und'][0]['value'];
    $data["home_team"] = $home_club->title;
    $data["home_team_image"] = file_create_url($home_club->field_club_image['und'][0]['uri']);
    $data["home_team_points"] = $club_details["$target_id"]->field_home_club_points['und'][0]['value'];
    $data["home_team_goals"] = $club_details["$target_id"]->field_home_club_goals['und'][0]['value'];
    $data["opponent_team_id"] =$club_details["$target_id"]->field_opponent_club_id['und'][0]['value'];
    $data["opponent_team"] =$opp_club->title;
    $data["opp_team_image"] = file_create_url($opp_club->field_club_image['und'][0]['uri']);
    $data["opp_team_points"] = $club_details["$target_id"]->field_opponent_club_points['und'][0]['value'];
    $data["opp_team_goals"] = $club_details["$target_id"]->field_opponent_club_goals['und'][0]['value'];

    return $data;

}




function gather_mediator_match_data_resource_update($nid,$node){

    global $user;
    $user_load = user_load($user->uid);
    if($user->uid <= 0){
        $error = "Access Denied for anonymous User";
        return services_error(t("Match Error"),406,array("club_match_error"=>$error));
    }

    if(empty($nid)){
        $error = "Please provide the match id";
        return services_error(t("Match Error"),406,array("club_match_error"=>$error));
    }
    $match = node_load($nid);
    $node_wrapper = entity_metadata_wrapper('node', $match);
    if(isset($node['match_likes'])) {

        $match_likes = $match->field_match_likes['und'][0]['value'] + 1;
        $node_wrapper->field_match_likes->set($match_likes);
        $node_wrapper->save();
        return array('match_likes'=>$match->field_match_likes['und'][0]['value']);
    }
    else {


        if (isset($node['field_home_club_points']) || isset($node['field_home_club_goals']) || isset($node['field_opponent_club_points']) || isset($node['field_opponent_club_goals'])) {
            $club_team_details = $node_wrapper->field_club_detail->value();

                $collection = entity_metadata_wrapper('field_collection_item', $club_team_details);

                $collection->field_home_club_points = $node['field_home_club_points'];
                $collection->field_home_club_goals = $node['field_home_club_goals'];
                $collection->field_opponent_club_points = $node['field_opponent_club_points'];
                $collection->field_opponent_club_goals = $node['field_opponent_club_goals'];
                $collection->save();


        }
        $i = 0;
        if (isset($node['field_home_team_detail']) && !empty($node['field_home_team_detail'])) {
            $home_team_details = $node_wrapper->field_home_team_detail->value();
            foreach ($home_team_details as $key => $home_team_fields) {
                $collection = entity_metadata_wrapper('field_collection_item', $home_team_fields);
                $collection->field_tid = $node['field_home_team_detail'][$i]['team_id'];
                $collection->field_tname = $node['field_home_team_detail'][$i]['team_player'];
                $collection->field_tnumber = $node['field_home_team_detail'][$i]['field_team_jersey_number'];
                $collection->save();
                $i++;
            }
        }
        $i = 0;
        if (isset($node['field_opponent_team_detail']) && !empty($node['field_opponent_team_detail'])) {
            $opp_team_details = $node_wrapper->field_opponent_team_detail->value();
            foreach ($opp_team_details as $key => $home_team_fields) {
                $collection = entity_metadata_wrapper('field_collection_item', $home_team_fields);
                $collection->field_tid = $node['field_opponent_team_detail'][$i]['team_id'];
                $collection->field_tname = $node['field_opponent_team_detail'][$i]['team_player'];
                $collection->field_tnumber = $node['field_opponent_team_detail'][$i]['field_team_jersey_number'];

                $collection->save();
                $i++;
            }
        }

        return true;
    }
    return false;


}


function gather_mediator_match_data_resource_access($op = 'view', $args = array())
{

    // Adds backwards compatability with regression fixed in #1083242
    /*if (isset($args[0])) {
        $args[0] = _services_access_value($args[0], 'node');
    }

    // Make sure we have an object or this all fails, some servers can
    // mess up the types.
    if (is_array($args[0])) {
        $args[0] = (object) $args[0];
    }
    elseif (!is_array($args[0]) && !is_object($args[0])) {  //This is to determine if it is just a string happens on node/%NID
        $args[0] = (object)array('nid' => $args[0]);
    }

    if ($op != 'create' && !empty($args)) {
        $node = node_load($args[0]->nid);
    }
    elseif ($op == 'create') {
        if (isset($args[0]->type)) {
            $node = $args[0]->type;
            return node_access($op, $node);
        }
        else {
            return services_error(t('Node type is required'), 406);
        }
    }
    if (isset($node->nid) && $node->nid) {
        return node_access($op, $node);
    }
    else {
        return services_error(t('Node @nid could not be found', array('@nid' => $args[0]->nid)), 404);
    }*/

    /*if (isset($args[0])) {
        $args[0] = _services_access_value($args[0], array('account', 'data'));
    }*/

    // Check if the user exists if appropriate.
    /*if ($op != 'create' && $op != 'register') {
        $account = user_load($args[0]);
        if (!$account) {
            return services_error(t('There is no user with ID @uid.', array('@uid' => $args[0])), 406);
        }
    }*/

    global $user;
    $account = user_load($user->uid);
    switch ($op) {
        case 'view':
           // return user_view_access($account);
        case 'update':
            return ($user->uid == $account->uid || user_access('administer users'));
        case 'create':
        case 'register':
            if (!$user->uid && variable_get('user_register', USER_REGISTER_VISITORS_ADMINISTRATIVE_APPROVAL) != USER_REGISTER_ADMINISTRATORS_ONLY) {
                return TRUE;
            } else {
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