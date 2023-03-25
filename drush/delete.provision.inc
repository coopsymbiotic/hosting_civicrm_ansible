<?php

/**
 * Implements hook_pre_provision_delete().
 *
 * Run CiviCRM Ansible playbooks with the 'civicrm-sites-delete' tag.
 * We run before Aegir, because in post the site does not exist anymore.
 */
function drush_provision_civicrm_ansible_pre_provision_delete($url = NULL) {
  if (d()->type == 'site') {
    ob_start();
    passthru('sudo /usr/local/bin/aegir-ansible ' . escapeshellarg(d()->uri) . ' civicrm-sites-delete');
    $output = ob_get_clean();

    drush_log(dt("CiviCRM Ansible: !output", ['!output' => '<pre>' . $output . '</pre>']), 'ok');
  }
}
