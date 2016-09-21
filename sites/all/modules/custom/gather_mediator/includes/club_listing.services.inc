<?php
function gather_mediator_club_listing_services_resource() {
    $api = array(
        'flags_club_listing' => array(
            'operations' => array(
                'index' => array(
                    'help' => 'Club Listing',
                    'file' => array(
                        'type' => 'inc',
                        'module' => 'gather_mediator',
                        'name' => 'includes/club_listing.services',
                    ),
                    'callback' => 'gather_mediator_club_listing_resource_retrieve',
                    'access arguments' => array('access content'),
                    //'access arguments append' => FALSE,
                    'args' => array(
                        array(
                            'name' => 'territory',
                            'optional' => TRUE,
                            'type' => 'int',
                            'description' => 'The zero-based index of the page to get, defaults to 0.',
                            'default' => '0',
                            'source' => array('param' => 'territory'),
                        ),
                    ),

                ),
            ),
        ),

    );
    return $api;
}

function gather_mediator_new_clubs_services_resource() {
    $api = array(
        'new_clubs' => array(
            'operations' => array(
                'index' => array(
                    'help' => 'Club Listing',
                    'file' => array(
                        'type' => 'inc',
                        'module' => 'gather_mediator',
                        'name' => 'includes/club_listing.services',
                    ),
                    'callback' => 'gather_mediator_new_clubs_resource_retrieve',
                    'access arguments' => array('access content'),
                    //'access arguments append' => FALSE,
                    'args' => array(
                        array(
                            'name' => 'last_cid',
                            'optional' => TRUE,
                            'type' => 'int',
                            'description' => 'The zero-based index of the page to get, defaults to 0.',
                            'default' => '0',
                            'source' => array('param' => 'last_cid'),
                        ),
                    ),

                ),
            ),
        ),

    );
    return $api;
}

function gather_mediator_clubs_fb_tw_services_resource() {
    $api = array(
        'fb_tw_id_clubs' => array(
            'operations' => array(
                'index' => array(
                    'help' => 'Club listing with fb and twid',
                    'file' => array(
                        'type' => 'inc',
                        'module' => 'gather_mediator',
                        'name' => 'includes/club_listing.services',
                    ),
                    'callback' => 'gather_mediator_fb_tw_resource_retrieve',
                    'access arguments' => array('access content'),
                    //'access arguments append' => FALSE,
                    'args' => array(
                        array(
                            'name' => 'club_id',
                            'optional' => TRUE,
                            'type' => 'int',
                            'description' => 'The zero-based index of the page to get, defaults to 0.',
                            'default' => '0',
                            'source' => array('param' => 'club_id'),
                        ),
                    ),

                ),
            ),
        ),

    );
    return $api;
}

function gather_mediator_club_listing_resource_retrieve($territory){

    global $user;

    $user_club_id = array();
    $user_club_nid = array();
    $i = 0;


    if($user->uid > 0) {
        $query = db_select('node', 'n');
        $query->fields('n', array('nid', 'title'));
        $query->fields('fm', array('uri'));
        $query->join('field_data_field_club_image', 'fcm', 'n.nid= fcm.entity_id');
        $query->join('file_managed', 'fm', 'fm.fid= fcm.field_club_image_fid');
        $query->join('field_data_field_list_of_clubs', 'fc', 'n.nid= fc.field_list_of_clubs_target_id');

        if(isset($_GET['territory']) && !empty($_GET['territory'])) {

            $query->join('field_data_field_territory', 't', 't.entity_id=n.nid');
            $query->condition('t.field_territory_target_id',$_GET['territory'],"=");
        }

        $query->condition('n.type', "clubs", "=");
        $query->condition('fc.entity_id', $user->uid, "=");

        $result = $query->execute();
        $results = $result->fetchAll();


        /*if(!$results){
            $error = "No Club is selected under this territory, Please select at least one club";
            return services_error(t("Club Listing Error"),403,array("club_listing_error"=>$error));
        }*/

        if(!empty($results)) {
            foreach ($results as $result) {
                $user_club_nid[$i]['nid'] = $result->nid;
                $user_club_id[] = $result->nid;
                $user_club_nid[$i]['title'] = $result->title;
                $user_club_nid[$i]['image'] = '';
                if (isset($result->uri) && !empty($result->uri)) {
                    $user_club_nid[$i]['image'] = file_create_url($result->uri);
                }
                $user_club_nid[$i]['follow_flag'] = "1";
                $i++;
            }
        }

        $query2 = db_select('node', 'n');
        $query2->fields('n', array('nid', 'title'));
        $query2->fields('fm', array('uri'));
        $query2->join('field_data_field_club_image', 'fcm', 'n.nid= fcm.entity_id');
        $query2->join('file_managed', 'fm', 'fm.fid= fcm.field_club_image_fid');

        if(isset($_GET['territory']) && !empty($_GET['territory'])) {

            $query2->join('field_data_field_territory', 't', 't.entity_id=n.nid');
            $query2->condition('t.field_territory_target_id',$_GET['territory'], "=");
        }

        $query2->condition('n.type', "clubs", "=");
        if(!empty($results)) {

            $query2->condition('n.nid', $user_club_id, "NOT IN");
        }
      // $query2->orderBy('title', 'ASC');

        $result2 = $query2->execute();
        $results2 = $result2->fetchAll();
       // print_r($results2);die;
        if(!empty($results2)) {
            foreach ($results2 as $result) {
                $user_club_nid[$i]['nid'] = $result->nid;
                $user_club_nid[$i]['title'] = $result->title;
                $user_club_nid[$i]['image'] = '';
                if (isset($result->uri) && !empty($result->uri)) {
                    $user_club_nid[$i]['image'] = file_create_url($result->uri);
                }
                $user_club_nid[$i]['follow_flag'] = "0";
                $i++;
            }
        }

        usort($user_club_nid, "clubid_sort");
        return $user_club_nid;

    }
    else{
        $error = "Access Denied for Anonymous User";
        return services_error(t("Club Listing Error"),403,array("club_listing_error"=>$error));

    }


}
function clubid_sort($a, $b) {
    return $a["nid"] - $b["nid"];
}
function gather_mediator_new_clubs_resource_retrieve($last_cid){
    global $user;

    if($user->uid <= 0) {
        $error = "Access Denied for Anonymous User";
        return services_error(t("Club Listing Error"),403,array("club_listing_error"=>$error));

    }
    $nodes = db_select('node','n')
        ->fields('n',array('nid'))
        ->condition('type','clubs')
        ->condition('nid',$last_cid,'>')
        ->condition('status',0,'>')
        ->execute()->fetchAll();

    $i = 0;
    $data = array();
    if(empty($nodes)){
        $data['status'] = 'No New Clubs Found';
        $data['new_clubs'] = '0';
        return $data;

    }

    foreach($nodes as $node_id){
        $node = node_load($node_id->nid);
        $data[$i]['nid'] = $node->nid;
        $data[$i]['title'] = $node->title;
        $data[$i]['image'] = file_create_url($node->field_club_image['und'][0]['uri']);
        $data[$i]['follow_flag'] = '0';

        $i++;
    }


    return $data;

}

function gather_mediator_fb_tw_resource_retrieve($club_id){
    $nodes = node_load($club_id);
    //print_r($nodes->field_club_fb_page_id['und'][0]['value']);die;
    $data['club_id'] = $nodes->nid;
    $data['club_fb_page_id'] = $nodes->field_club_fb_page_id['und'][0]['value'];
    $data['club_twitter_page_id'] = $nodes->field_club_twitter_page_id['und'][0]['value'];
    return $data;

}

function club_listing_services_resource_access($op = 'view', $args = array()) {
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